#include "SM4_lookup_table.h"
#include <stdint.h>
//密钥扩展的移位异或
uint32_t RotXor_key_schedule(uint32_t X) {
	return X ^ ROL(X, 13) ^ ROL(X, 23);
}
//创建密钥扩展T变换的四个表，表规模8bit->32bit
void building_key_tabT() {
	for (int i = 0;i < 4;i++) {
		for (int j = 0;j < 256;j++) {
			key_tabT[i][j] = RotXor_key_schedule((uint32_t)(SBox[j] << ((3 - i) * 8)));
		}
	}
}
//密钥扩展算法
void SM4_key_schedule_lookup_table(uint32_t* key, uint32_t* rk) {
	uint32_t K[4];//临时变量，存储当前4个状态
	uint32_t K_4;//存储更新的状态
	int i = 0;
	for (i = 0; i < 4; i++) K[i] = key[i] ^ FK[i];//状态填充
	//32轮产生轮密钥
	for (i = 0; i < 32; i++) {
		K_4 = K[1] ^ K[2] ^ K[3] ^ CK[i];//分支异或
		K_4 = key_tabT[0][0xff&(K_4>>24)] ^ key_tabT[1][0xff & (K_4 >> 16)] ^ key_tabT[2][0xff & (K_4 >> 8)] ^ key_tabT[3][0xff & K_4];//查表实现密钥扩展T变换
		K_4 ^= K[0];
		rk[i] = K_4;//保存第i+1轮的轮密钥
		//更新状态
		K[0] = K[1];
		K[1] = K[2];
		K[2] = K[3];
		K[3] = K_4;
	}
}