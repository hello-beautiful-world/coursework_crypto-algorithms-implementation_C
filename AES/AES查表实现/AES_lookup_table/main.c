#include"aes_lookup_table.h"
#include<stdio.h>
#include <stdint.h>
#include<time.h>
#include<math.h>
int main() {
	double  count = pow(2, 20);//加密次数，总加密16MB明文
	uint8_t pt[16], ct[16], key[16], rk[11][16];
	clock_t start, end;
	char char_pt[] = " 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
	char char_key[] = "00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00";
	int i;
	for (i = 0; i < 16; i++) {
		// 使用sscanf从字符串中读取明文和种子密钥
		sscanf_s(char_pt + i * 3, "%2hhx", pt + i);
		sscanf_s(char_key + i * 3, "%2hhx", key + i);
	}
	printf("待加密的明文为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", pt[i]);
	printf("\n种子密钥为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", key[i]);
	start = clock(); // 记录开始时间
	//密钥扩展算法
	aes_key_schedule(key, rk);
	printf("\n轮密钥为：\n");
	for (int r = 0; r < 11; r++) {
		for (i = 0; i < 16; i++) printf("%02x ", rk[r][i]);
		printf("\n");
	}
	//加密
	MC_build_tabla();//建表
	SB_MC_build_tabla();
	while (count--)
	{
		aes_encrypt(pt, rk, ct);
	}
	end = clock(); // 记录结束时间
	printf("加密所获得的密文为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", ct[i]);
	double time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("\n加密16MB的明文程序的运行时间为 %.4f 秒\n", time);//CLOCKS_PER_SEC是time.h定义的宏,CPU运行时钟周期数/s。
	printf("运行速度为 % .4f Mbps\n", 16*8 / time);
	//解密
	Inv_T_build_tabla();//创建逆MC表
	Inv_TS_build_tabla();//创建逆SB、MC表
	aes_decrypt(ct, rk, pt);
	printf("\n解密所获得的明文为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", pt[i]);
	return 0;
}