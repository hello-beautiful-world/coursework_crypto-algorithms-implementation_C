#include "Skinny128_256.h"
#include <stdint.h>
#include<stdio.h>
#include<time.h>
#include<math.h>
#include<string.h>
int main() {
	double  count = pow(2, 20);//加密次数，总加密16MB明文
	clock_t start, end;//用于计时
	char str_pt[] = "3a0c47767a26a68dd382a695e7022e25";
	char str_key[] = "009cec81605d4ac1d2ae9e3085d7a1f31ac123ebfc00fddcf01046ceeddfcab3";
	uint8_t pt[16];//128bits明文,使用uint8_t*16存储
	uint8_t de_pt[16];//解密获得的128bits明文,使用uint8_t*16存储
	uint8_t key[2][16];//256bits初始可调密钥TK1、TK2(分别为128bits)
	uint8_t ct[16];//128bits密文,使用uint8_t*16存储
	uint8_t rk[SKINNY_ROUNDS][2][8];//存储SKINNY_ROUNDS轮（48轮）的可调密钥TK1、TK2的前两行，8*8bits=64bits
	//处理明文和初始可调密钥字符串
	for (int i = 0; i < 16; i++) {
		sscanf_s(str_pt + 2 * i, "%2hhx", pt + i);//pt
		sscanf_s(str_key + 2 * i, "%2hhx", key[0] + i);//TK1
		sscanf_s(str_key + 32 + 2 * i, "%2hhx", key[1] + i);//TK2
	}
	printf("待加密的明文：\n");
	for (int i = 0; i < 16; i++) printf("%02x", pt[i]);
	printf("\n初始可调密钥TK1：\n");
	for (int i = 0; i < 16; i++) printf("%02x", key[0][i]);
	printf("\n初始可调密钥TK2：\n");
	for (int i = 0; i < 16; i++) printf("%02x", key[1][i]);

	//6比特线性移位寄存器生成轮常量
	Skinny128_256_constant_schedule(constant);
	printf("\nConstants：\n");
	for (int r = 0; r < SKINNY_ROUNDS; r++) {
		printf("%02x ", constant[r]);
		if ((r + 1) % 16 == 0) printf("\n");
	}
	//创建S盒、逆S盒
	building_Sbox();
	start = clock(); // 记录开始时间
	//可调密钥编排算法
	Skinny128_256_tweakey_schedule(key, rk);
	/*printf("\n轮可调密钥为：");
	for (int r = 0; r < SKINNY_ROUNDS; r++) {
		printf("\n第%d轮：", r + 1);
		for (int i = 0; i < 8; i++) printf("%02x", rk[r][0][i]);
		printf("  ");
		for (int i = 0; i < 8; i++) printf("%02x", rk[r][1][i]);
	}*/
	//加密
	//while (count--)
	Skinny128_256_encrypt(pt, rk, ct);
	end = clock(); // 记录结束时间
	printf("\n\n加密所获得的密文为：\n");
	for (int i = 0; i < 16; i++) printf("%02x ", ct[i]);
	double time = ((double)(end - start)) / CLOCKS_PER_SEC;
	printf("\n\n加密16MB的明文程序的运行时间为 %.4f 秒\n", time);//CLOCKS_PER_SEC是time.h定义的宏,CPU运行时钟周期数/s。
	printf("运行速度为 % .4f Mbps\n", 16 * 8 / time);
	//解密
	Skinny128_256_decrypt(ct, rk, de_pt);
	printf("\n解密所获得的明文为：\n");
	for (int i = 0; i < 16; i++) printf("%02x ", de_pt[i]);
	printf("\n");
	return 0;
}