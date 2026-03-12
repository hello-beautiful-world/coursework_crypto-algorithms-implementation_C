//#include "Rectangle80_bit_slice.h"
#include "Rectangle80_lookup_table.h"
#include <stdint.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<string.h>
int main() {
	double  count = pow(2, 20);//加密次数，总加密8MB明文
	clock_t start, end;//用于计时
	int pt[64] = { 0 };//明文,pt[0]为最高位,便于查表
	int ct[64];//密文
	int key[80] = { 0 };//种子密钥80bit,密钥扩展基于比特实现
	int rk[26][64] = { 0 };//轮密钥,64bit*26个,初始化为全0
	int i;
	build_tables_8bit();//建8bit表
	printf("待加密的明文为（按高低位输出）：\n");
	for (i = 0; i < 64; i++) {
		printf("%d", pt[i]);
		if ((i + 1) % 16 == 0) printf(" ");
	}
	printf("\n种子密钥为（按高低位输出）：\n");
	for (i = 0; i < 80; i++) {
		printf("%d", key[i]);
		if ((i + 1) % 16 == 0) printf(" ");
	}
	RC_transf();//处理密钥扩展轮常量
	//密钥扩展算法
	Rectangle80_lookup_table_key_schedule(key, rk);
	printf("\n轮密钥为(按高低位输出):");
	for (int r = 0; r < 26; r++) {
		printf("\n第%d轮:", r);
		for (i = 0; i < 64; i++) {
			printf("%d", rk[r][i]);
			if ((i + 1) % 16 == 0) printf(" ");
		}
	}
	//处理中间24个轮密钥,拉线
	for (i = 1; i < 25; i++) {
		P0(rk[i]);
	}
	start = clock(); // 记录开始时间
	//加密
	/*
	while (count--)
	{
		Rectangle80_lookup_table_encrypt(pt, rk, ct);
	}*/
	 Rectangle80_lookup_table_encrypt(pt, rk, ct);
	end = clock(); // 记录结束时间
	printf("\n\n加密所获得的密文为（按高低位输出）：\n");
	for (i = 0; i < 64; i++) {
		printf("%d", ct[i]);
		if ((i + 1) % 16 == 0) printf(" ");
	}
	double time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("\n\n加密8MB的明文程序的运行时间为 %.4f 秒\n", time);//CLOCKS_PER_SEC是time.h定义的宏,CPU运行时钟周期数/s。
	printf("运行速度为 % .4f Mbps\n", 8 * 8 / time);
	return 0;
	/*比特切片
	double  count = pow(2, 20);//加密次数，总加密8MB明文
	clock_t start, end;//用于计时
	uint8_t pt_8[8] = {0x00};
	uint16_t pt[4];//明文
	uint16_t ct[4];//密文
	int key[80] = { 0 };//种子密钥80bit,密钥扩展基于比特实现
	uint16_t rk[26][4] = {0};//轮密钥,64bit*26个,初始化为全0
	int i;
	//装载明文
	for (i = 0; i < 4; i++) {
		pt[3 - i] = ((uint16_t)pt_8[2 * i] << 8) | ((uint16_t)pt_8[2 * i+1]);
	}
	printf("待加密的明文为（按高低位输出）：\n");
	for (i = 0; i < 8; i++) printf("%02x ", pt_8[i]);
	printf("\n种子密钥为（按高低位输出）：\n");
	for (i = 0; i < 80; i++) {
		printf("%d ", key[i]);
		if ((i + 1) % 16 == 0) printf("\n");
	}
	RC_transf();//处理密钥扩展轮常量
	start = clock(); // 记录开始时间
	//密钥扩展算法
	Rectangle80_key_schedule(key, rk);
	printf("轮密钥为(按矩阵输出):\n");
	for (int r = 0; r < 26; r++) {
		printf("第%d轮:", r);
		for (i = 0; i < 4; i++) {
			printf("%04x ", rk[r][i]);
		}
		if ((r + 1) % 3 == 0) printf("\n");
	}
	//加密
	while (count--)
	{
		Rectangle80_bit_slice_encrypt(pt, rk, ct);
	}
	end = clock(); // 记录结束时间
	printf("\n\n加密所获得的密文为（按矩阵输出）：\n");
	for (i = 0; i < 4; i++) printf("%04x ", ct[i]);
	double time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("\n\n加密8MB的明文程序的运行时间为 %.4f 秒\n", time);//CLOCKS_PER_SEC是time.h定义的宏,CPU运行时钟周期数/s。
	printf("运行速度为 % .4f Mbps\n", 8 * 8 / time);
	return 0;*/
}
