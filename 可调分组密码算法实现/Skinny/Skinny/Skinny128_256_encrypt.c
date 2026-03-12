#include "Skinny128_256.h"
#include <stdint.h>
#include <string.h>
//创建S盒
void building_Sbox() {
	int T[8] = { 5,3,0,4,6,7,1,2 };//bit permutation表
	//8bits可以表示256种值
	for (int i = 0; i < 256; i++) {
		int arr[8];//存储i的二进制表达，arr[0]为最低位
		int pu_arr[8];//存储经过一遍过程的arr数组
		for (int j = 0; j < 8; j++)	arr[j] = (i >> j) & 1;
		for (int j = 0; j < 3; j++) {//执行3次
			arr[4] = arr[4] ^ ((~(arr[7] | arr[6])) & 1);
			arr[0] = arr[0] ^ ((~(arr[3] | arr[2])) & 1);
			for (int k = 0; k < 8; k++) pu_arr[k] = arr[T[k]];//注意arr[0]为最低位
			memcpy(arr, pu_arr, 8 * sizeof(int));
		}
		arr[4] = arr[4] ^ ((~(arr[7] | arr[6])) & 1);
		arr[0] = arr[0] ^ ((~(arr[3] | arr[2])) & 1);
		int tmp = arr[1];//交换arr[1]和arr[2]
		arr[1] = arr[2];
		arr[2] = tmp;
		uint8_t s = 0;
		for (int j = 0; j < 8; j++) s = (s << 1) | arr[7 - j];
		Sbox[i] = s;
	}
	printf("\nS盒：\n");
	for (int i = 0; i < 256; i++) {
		printf("%02x ",Sbox[i]);
		if ((i + 1) % 16 == 0) printf("\n");
	}
	for (int i = 0; i < 256; i++) inv_Sbox[Sbox[i]] = i;
	printf("\n逆S盒：\n");
	for (int i = 0; i < 256; i++) {
		printf("%02x ", inv_Sbox[i]);
		if ((i + 1) % 16 == 0) printf("\n");
	}
}
//Skinny128_256加密
void Skinny128_256_encrypt(uint8_t* pt, uint8_t rk[SKINNY_ROUNDS][2][8], uint8_t* ct) {
	memcpy(ct, pt, 16 * sizeof(uint8_t));
	for (int r = 0; r < SKINNY_ROUNDS; r++) {
		//SubCells(SC)
		for (int i = 0; i < 16; i++) ct[i] = Sbox[ct[i]];
		//AddConstants (AC)
		ct[0] ^= (constant[r] & 0xf);//异或c0=0000||rc3||rc2||rc1||rc0
		ct[4] ^= ((constant[r] >> 4) & 0x3);//异或c1=000000||rc5||rc4
		ct[8] ^= 0x2;//异或c2=0x2
		//AddRoundTweakey (ART)
		for (int i = 0; i < 8; i++) {
			ct[i] = ct[i]^rk[r][0][i]^ rk[r][1][i];
		}
		//ShiftRows (SR)
		int P[16] = {0, 1, 2, 3, 7, 4, 5, 6, 10, 11, 8, 9, 13, 14, 15, 12};
		uint8_t tmp[16];
		for (int i = 0; i < 16; i++) {
			tmp[i] = ct[P[i]];
		}
		memcpy(ct, tmp, 16 * sizeof(uint8_t));
		//MixColumns(MC)
		uint8_t ct2[16];//存储未MC的中间状态
		memcpy(ct2, ct, 16* sizeof(uint8_t));
		for (int i = 0; i < 4; i++) {
			ct[i] = ct2[i] ^ ct2[8 + i] ^ ct2[12 + i];
			ct[4 + i] = ct2[i];
			ct[8 + i] = ct2[4 + i] ^ ct2[8 + i];
			ct[12 + i] = ct2[i] ^ ct2[8 + i];
		}
		printf("\nr=%d\n", r);
		for (int i = 0; i < 16; i++) printf("%02x", ct[i]);
	}
}