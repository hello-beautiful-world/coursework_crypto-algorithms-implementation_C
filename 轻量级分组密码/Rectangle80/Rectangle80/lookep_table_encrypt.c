#include "Rectangle80_lookup_table.h"
#include <stdint.h>
#include<string.h>
#include<math.h>
//一轮异或轮密钥
void Xor_rk(int* ct, int(*rk)[64], int r) {
	for (int i = 0; i < 64; i++) {
		ct[i] ^= rk[r][i];
	}
}
void Rectangle80_lookup_table_encrypt(int* pt, int(*rk)[64], int* ct) {
	int r = 0;
	memcpy(ct, pt, 64*sizeof(int));
	//第一轮异或轮密钥
	Xor_rk(ct, rk, r);
	//第一轮拉线
	P0(ct);
	//24轮，通过查表实现
	for (r = 1; r < 25; r++) {
		int tmp[8][64];//存储查表结果
		//查表8次
		for (int i = 0; i < 8; i++) {
			//计算8bit的值，用于查表
			int value = 0;
			for (int j = 0; j < 8; j++) {
				value += (ct[8 * i + j] * pow(2, (7 - j)));
			}
			/*
			for (int j = 0; j < 64; j++) {
				tmp[i][j] = SPT_2[i][value][j];
			}*/
			memcpy(tmp[i], SPT_2[i][value], 64 * sizeof(int));
		}
		int tm[64] = { 0 };
		//8次查表的结果异或
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 64; j++) {
				tm[j] ^= tmp[i][j];
			}
		}
		memcpy(ct, tm, 64 * sizeof(int));
		//异或经过拉线操作的密钥
		Xor_rk(ct, rk, r);
	}
	//最后一轮
		//过8个S盒
	for (int i = 0; i < 16; i++) {
		int value = ct[4 * i] * 8 + ct[4 * i + 1] * 4 + ct[4 * i + 2] * 2 + ct[4 * i + 3];
		ct[4 * i] = Sbox_bit[value][0];
		ct[4 * i + 1] = Sbox_bit[value][1];
		ct[4 * i + 2] = Sbox_bit[value][2];
		ct[4 * i + 3] = Sbox_bit[value][3];
	}
		//逆拉线
	inv_P0(ct);
		//循环左移
	int tmp2[2][16];//将前32bit保存下来，便于循环移位
	memcpy(tmp2[0], ct, 16 * sizeof(int));//前16bit
	memcpy(tmp2[1], ct + 16, 16 * sizeof(int));
	int ct_32 = ct[32];//保存第32位，便于循环左移1位
	for (int k = 0; k < 16; k++) {
		//循环左移13位
		if (k < 3) ct[k] = tmp2[0][13 + k];
		else ct[k] = tmp2[0][k - 3];
		//循环左移12位
		if (k < 4) ct[16 + k] = tmp2[1][12 + k];
		else ct[16 + k] = tmp2[1][k - 4];
		//循环左移1位
		if (k == 15) ct[32 + k] = ct_32;//ct[47]=ct[32]
		else ct[32 + k] = ct[33 + k];
	}
	//异或最后一组轮密钥
	Xor_rk(ct, rk, r);
}