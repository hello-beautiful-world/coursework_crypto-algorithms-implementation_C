#include "Skinny128_256.h"
#include <stdint.h>
#include <string.h>

//Skinny128_256解密
void Skinny128_256_decrypt(uint8_t* ct, uint8_t rk[SKINNY_ROUNDS][2][8], uint8_t* de_pt) {
	memcpy(de_pt, ct, 16 * sizeof(uint8_t));
	for (int r = 0; r < SKINNY_ROUNDS; r++) {
		//Inv_MixColumns
		uint8_t tmp[16];
		memcpy(tmp, de_pt, 16 * sizeof(uint8_t));
		for (int i = 0; i < 4; i++) {
			de_pt[i] = tmp[4 + i];
			de_pt[4 + i] = tmp[4 + i] ^ tmp[8 + i] ^ tmp[12 + i];
			de_pt[8 + i] = tmp[4 + i] ^ tmp[12 + i];
			de_pt[12 + i] = tmp[i] ^ tmp[12 + i];
		}
		//Inv_ShiftRows (SR)
		int inv_P[16] = { 0,1,2,3,5,6,7,4,10,11,8,9,15,12,13,14 };
		for (int i = 0; i < 16; i++) {
			tmp[i] = de_pt[inv_P[i]];
		}
		memcpy(de_pt, tmp, 16 * sizeof(uint8_t));
		//Inv_AddRoundTweakey (ART)
		for (int i = 0; i < 8; i++) {
			de_pt[i] = de_pt[i] ^ rk[SKINNY_ROUNDS-1-r][0][i] ^ rk[SKINNY_ROUNDS-1-r][1][i];
		}
		//Inv_AddConstants (AC)
		de_pt[0] ^= (constant[SKINNY_ROUNDS - 1 - r] & 0xf);//异或c0=0000||rc3||rc2||rc1||rc0
		de_pt[4] ^= ((constant[SKINNY_ROUNDS - 1 - r] >> 4) & 0x3);//异或c1=000000||rc5||rc4
		de_pt[8] ^= 0x2;//异或c2=0x2
		//Inv_SubCells(SC)
		for (int i = 0; i < 16; i++) de_pt[i] = inv_Sbox[de_pt[i]];
	}
}