#include "SM4_lookup_table.h"
#include <stdint.h>
//将uint8_t X[16]转化位uint32_t Y[4]
void uint8_to_uint32(uint8_t* X, uint32_t* Y) {
	for (int i = 0; i < 4; i++) {
		Y[i] = (X[4 * i] << 24) ^ (X[4 * i + 1] << 16) ^ (X[4 * i + 2] << 8) ^ X[4 * i + 3];
	}
}
//将uint32_t X[4]转化位uint8_t Y[16]
void uint32_to_uint8(uint32_t* X, uint8_t* Y) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			Y[4 * i + j] = 0xff & (X[i] >> (8 * (3 - j)));
		}
	}
}
//加密的移位异或
uint32_t RotXor(uint32_t X) {
	return X ^ ROL(X, 2) ^ ROL(X, 10) ^ ROL(X, 18) ^ ROL(X, 24);
}
//创建加密T变换的四个表，表规模8bit->32bit
void building_en_tabT() {
	for (int i = 0;i < 4;i++) {
		for (int j = 0;j < 256;j++) {
			en_tabT[i][j] = RotXor((uint32_t)(SBox[j] << ((3 - i) * 8)));
		}
	}
}
//加解密流程相同，轮密钥顺序相反
void SM4_encrypt_lookup_table(uint8_t* pt, uint32_t* rk, uint8_t* ct) {
	uint32_t X[4];//临时变量，存储中间状态
	uint32_t X_4;//存储更新的状态
	uint8_to_uint32(pt, X);
	//迭代32轮
	for (int r = 0; r < 32; r++) {
		X_4 = X[1] ^ X[2] ^ X[3] ^ rk[r];//分支异或
		X_4 = en_tabT[0][0xff&(X_4>>24)]^ en_tabT[1][0xff & (X_4 >> 16)] ^ en_tabT[2][0xff & (X_4 >> 8)] ^ en_tabT[3][0xff&X_4] ;//查表实现T
		X_4 ^= X[0];
		//更新状态
		X[0] = X[1];
		X[1] = X[2];
		X[2] = X[3];
		X[3] = X_4;
	}
	uint32_t Y[4];//存储反序变换后的结果
	for (int i = 0; i < 4; i++) {
		Y[i] = X[3 - i];
	}
	uint32_to_uint8(Y, ct);
}