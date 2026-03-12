#include"aes_lookup_table.h"
#include <stdint.h>
#include <string.h>
uint32_t T[4][256] = {0};//MC表，4*8bit-->4*32bit
uint32_t TS[4][256] = {0};//SB、MC表，4*8bit-->4*32bit
//异或轮密钥(一轮)
void AddRoundKey(uint8_t* s, uint8_t* rk) {
	for (int i = 0; i < 16;i++) {
		s[i] ^= rk[i];
	}
}
//行移位，循环左移，用于最后一轮
void ShiftRow(uint8_t* s) {
	uint8_t tmp[16];//临时变量
	memcpy(tmp, s, 16);
	//第二行
	s[1] = tmp[5];
	s[5] = tmp[9];
	s[9] = tmp[13];
	s[13] = tmp[1];
	//第三行
	s[2] = tmp[10];
	s[6] = tmp[14];
	s[10] = tmp[2];
	s[14] = tmp[6];
	//第四行
	s[3] = tmp[15];
	s[7] = tmp[3];
	s[11] = tmp[7];
	s[15] = tmp[11];
}
//02*uint_8[4]
uint8_t xtime(uint8_t a) {
	return (a >> 7) == 1 ? (0xff & (((a) << 1) ^ 0x1B)) : ((a) << 1);
}
//03*uint_8[4]
uint8_t xtime_03(uint8_t a) {
	return xtime(a) ^ a;//02+01
}
//字节代换，用于最后一轮
void SubBytes(uint8_t* s) {
	for (int i = 0;i < 16;i++) {
		s[i] = Sbox[s[i]];
	}
}
//列混淆建表
void MC_build_tabla() {
	for (int i = 0;i < 256;i++) {
		T[0][i] = (((uint32_t)xtime_03(i)) << 24) ^ (((uint32_t)i) << 16) ^ (((uint32_t)i) << 8) ^ ((uint32_t)xtime(i));//03x,x,x,02x(矩阵第一列倒序),注意排在列上方的放右边
		T[1][i] = (T[0][i] << 8) ^ (T[0][i] >> 24);
		T[2][i] = (T[1][i] << 8) ^ (T[1][i] >> 24);
		T[3][i] = (T[2][i] << 8) ^ (T[2][i] >> 24);
	}
}
//字节代换、列混淆建表
void SB_MC_build_tabla() {
	for (int i = 0;i < 256;i++) {
		for (int j = 0; j < 4;j++) {//0~3
			TS[j][i] = T[j][Sbox[i]];
		}
	}
}

void aes_encrypt(uint8_t* pt, uint8_t(*rk)[16], uint8_t* ct) {
	uint32_t tmp[4] = { 0 };//临时变量，4*32bit
	memcpy(ct, pt, 16);
	//异或白化密钥
	AddRoundKey(ct, rk[0]);
	int r = 1;
	//9轮核心轮函数f
	for (r; r <= 9;r++) {
		//实现SR、SB、MC
		for (int i = 0;i < 4;i++) {
			tmp[i] = TS[0][ct[4*i]] ^ TS[1][ct[(5+4*i)%16]] ^ TS[2][ct[(10 + 4 * i) % 16]] ^ TS[3][ct[(15 + 4 * i) % 16]];//同时实现行移位
		}
		//4*32bit-->16*8bit
		for (int i = 0;i < 4;i++) {
			ct[4 * i] = tmp[i] & 0xff;//取最后8位
			ct[1 + 4 * i] = (tmp[i] >> 8 ) & 0xff;
			ct[2 + 4 * i] = (tmp[i] >> 16) & 0xff;
			ct[3 + 4 * i] = tmp[i] >> 24;
		}
		//轮密钥加
		AddRoundKey(ct, rk[r]);
	}
	SubBytes(ct);
	ShiftRow(ct);
	AddRoundKey(ct, rk[r]);
}