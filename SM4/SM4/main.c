#include "SM4.h"
#include <stdint.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<string.h>
int main() {
	double  count = pow(2, 20);//加密次数，总加密16MB明文
	clock_t start, end;//用于计时
	uint8_t pt[16], ct[16];//明文密文
	uint32_t key[4];//种子密钥
	uint32_t rk[32];//轮密钥
	char char_pt[] = "01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10";
	char char_key[] = "01 23 45 67 89 ab cd ef fe dc ba 98 76 54 32 10";
	// 使用sscanf从字符串中读取明文
	int i;
	for (i = 0; i < 16; i++) {
		sscanf_s(char_pt + i * 3, "%2hhx", pt + i);
	}
	// 使用sscanf从字符串中种子密钥
	char new_key[33];//临时变量，用于存储char_key去掉空格的结果，便于后于转化为uint32_t类型
	int j = 0;
	for (i = 0;i < strlen(char_key);i++) {//去除空格
		if (char_key[i] != ' ') {
			new_key[j] = char_key[i];
			j++;
		}
	}
	new_key[j] = '\0';
	sscanf_s(new_key, "%8x%8x%8x%8x", key, key + 1, key + 2, key + 3);
	//输出明文和种子密钥
	printf("待加密的明文为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", pt[i]);
	printf("\n种子密钥为：\n");
	for (i = 0; i < 4; i++) printf("%08x ", key[i]);
	start = clock(); // 记录开始时间
	//密钥扩展算法
	SM4_key_schedule(key, rk);
	printf("\n轮密钥为：\n");
	for (int r = 0; r < 32; r++) {
		printf("%08x  ", rk[r]);
		if ((r+1) % 8 == 0) printf("\n");
	}
	//加密
	while (count--)
	{
		SM4_encrypt(pt, rk, ct);
	}
	end = clock(); // 记录结束时间
	printf("\n\n加密所获得的密文为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", ct[i]);
	double time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("\n\n加密16MB的明文程序的运行时间为 %.4f 秒\n", time);//CLOCKS_PER_SEC是time.h定义的宏,CPU运行时钟周期数/s。
	printf("运行速度为 % .4f Mbps\n", 16 * 8 / time);
	//解密
	for (int i = 0; i < 16; i++) {//将轮密钥反序用于解密
		uint32_t tmp = rk[i];
		rk[i] = rk[31 - i];
		rk[31 - i] = tmp;
	}
	SM4_encrypt(ct, rk, pt);
	printf("\n解密所获得的明文为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", pt[i]);
	printf("\n");
	return 0;
}
