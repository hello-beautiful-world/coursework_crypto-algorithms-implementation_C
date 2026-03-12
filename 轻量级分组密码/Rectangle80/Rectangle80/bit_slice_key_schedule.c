#include "Rectangle80_bit_slice.h"
#include<stdint.h>
//从80bit寄存器中取出第i轮的轮密钥
void get_rk(int* key, uint16_t(*rk)[4], int r) {
	uint16_t x = 0x0001;
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 16; j++) {
			if (key[16 * i + j] == 1) {
				rk[r][i] ^=(x << (15 - j));
			}
		}
	}
}
//密钥扩展四个S盒变换
void S(int* key) {
	for (int i = 0; i < 4; i++) {
		int x0, x1, x2, x3;
		x0 = key[15 - i];
		x1 = key[31 - i];
		x2 = key[47 - i];
		x3 = key[63 - i];
		key[15 - i] = (x0 + x0 * x1 + x2 + x3) % 2;//注意int需要模2
		key[31 - i] = (1 + x0 + x1 + x2 + x1 * x3) % 2;
		key[47 - i] = (1 + x0 * x1 + x2 + x0 * x2 + x1 * x2 + x0 * x1 * x2 + x3 + x2 * x3) % 2;
		key[63 - i] = (x1 + x0 * x2 + x1 * x2 + x3 + x0 * x3 + x1 * x2 * x3) % 2;
	}
}
//密钥扩展的行变换
void row_ope(int* key) {
	int tmp_k0[16];//临时变量，用于存储K[0]
	int tmp_0[16];//临时变量，用于存储K[0]<<<8
	int tmp_3[16];//临时变量，用于存储K[3]<<<12
	for (int i = 0; i < 16; i++) {
		tmp_k0[i] = key[i];//存储前16比特
		tmp_0[i] = key[(8 + i) % 16];//存储K[0]<<<8
		if (i < 4) tmp_3[i] = key[60 + i];//存储K[3]<<<12
		else tmp_3[i] = key[44 + i];//44+i=48+i-4
	}
	for (int i = 0; i < 16; i++) {
		key[i] = tmp_0[i] ^ key[16 + i];//K_0^′=(K_0⋘8)⊕K_1
		key[16 + i] = key[32 + i];//K_1^′=K_2
		key[32 + i] = key[48 + i];//K_2^′=K_3
		key[48 + i] = tmp_3[i] ^ key[64 + i];//K_3^′=(K_3⋘12)⊕K_4
		key[64 + i] = tmp_k0[i];//K_4^′=K_0
	}
}
//密钥扩展异或轮常数
void Xor_constant(int* key,int r){
	for (int i = 0; i < 5; i++) {
		key[11 + i] ^= RC_bit[r][i];
	}
}
//密钥扩展算法
void Rectangle80_key_schedule(int* key, uint16_t* rk[][4]) {
	int K[80];
	//填充K=K_4∥K_3∥K_2∥K_1∥K_0,注意高低位
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 16; j++) {
			K[16 * i + j] = key[16 * (4 - i) + j];
		}
	}
	get_rk(K, rk, 0);//取白化密钥
	//25次更新移位寄存器
	for (int r = 1; r < 26; r++) {
		S(K);//4个s盒变换
		row_ope(K);//行变换
		Xor_constant(K, r - 1);//异或轮常数
		get_rk(K, rk, r);//取密钥
	}
}