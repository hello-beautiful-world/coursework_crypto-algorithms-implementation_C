#include "Simon128_128.h"
#include <stdint.h>
#include<stdio.h>
#include<time.h>
#include<math.h>

int main() {
	double  count = pow(2, 20);//加密次数，总加密16MB明文
	clock_t start, end;//用于计时
	uint64_t rk[SIMON_ROUNDS];//轮密钥，64bit*68轮
	uint64_t key[2] = {0x0f0e0d0c0b0a0908 , 0x0706050403020100};//种子密钥,共128bit
	uint8_t pt[16], ct[16];//明文密文
	char char_pt[] = "63 73 65 64 20 73 72 65 6c 6c 65 76 61 72 74 20";
	// 使用sscanf从字符串中读取明文
	int i;
	for (i = 0; i < 16; i++) {
		sscanf_s(char_pt + i * 3, "%2hhx", pt + i);
	}
	//输出明文和种子密钥
	printf("待加密的明文为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", pt[i]);
	printf("\n种子密钥为：\n");
	for (i = 0; i < 2; i++) printf("%llx ", key[i]);
	start = clock(); // 记录开始时间
	//密钥扩展算法
	Simon_128_128_key_schedule(key, rk);
	printf("\n轮密钥为：\n");
	for (int r = 0; r < SIMON_ROUNDS; r++) {
		printf("%llx  ", rk[r]);
		if ((r + 1) % 4 == 0) printf("\n");
	}
	//加密
	while (count--)
	{
		Simon_128_128_encrypt(pt, rk, ct);
	}
	end = clock(); // 记录结束时间
	printf("\n加密所获得的密文为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", ct[i]);
	double time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("\n\n加密16MB的明文程序的运行时间为 %.4f 秒\n", time);//CLOCKS_PER_SEC是time.h定义的宏,CPU运行时钟周期数/s。
	printf("运行速度为 % .4f Mbps\n", 16 * 8 / time);
	
	//解密
	for (int i = 0; i < SIMON_ROUNDS/2; i++) {//将轮密钥反序用于解密
		uint64_t tmp = rk[i];
		rk[i] = rk[SIMON_ROUNDS-1 - i];
		rk[SIMON_ROUNDS-1 - i] = tmp;
	}
	Simon_128_128_encrypt(ct, rk, pt);
	printf("\n解密所获得的明文为：\n");
	for (i = 0; i < 16; i++) printf("%02x ", pt[i]);
	printf("\n");
	return 0;
}