#include "Skinny128_256.h"
#include <stdint.h>
#include <string.h>
//PT字置换操作：
void permutate(uint8_t* key) {
	uint8_t tmp[16];
	for (int i = 0; i < 16; i++) {
		tmp[i] = key[PT[i]];
	}
	memcpy(key, tmp, 16 * sizeof(uint8_t));
}
//LFSR操作,作用于前两行的每一个字
void LFSR(uint8_t* key) {
	//作用于前两行的每一个字
	for (int i = 0; i < 8; i++) {
		key[i] = (key[i] << 1) ^ ((key[i] >> 7) ^ ((key[i] >> 5) & 0x01));//(x6||x5||x4||x3||x2||x1||x0||x7 ⊕ x5)
	}
}
//可调密钥编排算法
void Skinny128_256_tweakey_schedule(uint8_t (*key)[16], uint8_t rk[SKINNY_ROUNDS][2][8]) {
	//存储初始可调密钥
	for (int i = 0; i < 8; i++) {
		rk[0][0][i] = key[0][i];//TK1
		rk[0][1][i] = key[1][i];//TK2
	}
	for (int r = 1; r < SKINNY_ROUNDS; r++) {
		//TK1,注意LFSR操作不作用于TK1.
		permutate(key[0]);
		//TK2
		permutate(key[1]);
		LFSR(key[1]);
		//存储TK1\TK2前2行
		for (int i = 0; i < 8; i++) {
			rk[r][0][i] = key[0][i];//TK1
			rk[r][1][i] = key[1][i];//TK2
		}
	}
}