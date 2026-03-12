#include "Present80.h"
#include <stdint.h>
#include<stdio.h>
#include<time.h>
#include<math.h>

int main() {
	double  count = pow(2, 20);//加密次数，总加密16MB明文
	clock_t start, end;//用于计时
	uint64_t rk[32];//轮密钥，64bit*(31轮+1白化密钥）
	uint64_t key_0 = 0xffffffffffffffff;//种子密钥第一部分,64bit
	uint16_t key_1 = 0xffff;//种子密钥第二部分,16bit，共80bit
	uint8_t pt[8] = { 0 };//明文，64bit
	uint8_t	ct[8];//密文，64bit
	//输出明文和种子密钥
	printf("待加密的明文为：\n");
	for (int i = 0; i < 8; i++) printf("%02x ", pt[i]);
	printf("\n种子密钥为：\n");
	printf("%llx ", key_0);
	printf("%hx ", key_1);
	building_Tab();//建SP变换表
	start = clock(); // 记录开始时间
	//密钥扩展算法
	Present80_key_schedule(key_0, key_1, rk);
	printf("\n轮密钥为：\n");
	for (int r = 0; r < 32; r++) {
		printf("%llx  ", rk[r]);
		if ((r + 1) % 4 == 0) printf("\n");
	}
	
	//加密
	while (count--)
	{
		Present80_encrypt(pt, rk, ct);
	}
	end = clock(); // 记录结束时间
	printf("\n加密所获得的密文为：\n");
	for (int i = 0; i < 8; i++) printf("%02x ", ct[i]);
	double time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("\n\n加密16MB的明文程序的运行时间为 %.4f 秒\n", time);//CLOCKS_PER_SEC是time.h定义的宏,CPU运行时钟周期数/s。
	printf("运行速度为 % .4f Mbps\n", 16 * 8 / time);
	return 0;
}