#include"aes.h"
#include <stdint.h>
#include <string.h>
//异或轮密钥(一轮)
void AddRoundKey(uint8_t* s, uint8_t* rk) {
	for (int i = 0; i < 16;i++) {
		s[i]  ^= rk[i];
	}
}
//行移位，循环左移
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
//列混淆
void MixColumns(uint8_t* s) {
	uint8_t tmp[16];//临时变量
	memcpy(tmp, s, 16);
	for (int i = 0;i < 4;i++) {
		s[0 + 4 * i] = xtime(tmp[4 * i]) ^ xtime(tmp[1+ 4 * i]) ^ tmp[1 + 4 * i] ^ tmp[2 + 4 * i] ^ tmp[3 + 4 * i];//第一行
		s[1 + 4 * i] = tmp[4 * i] ^ xtime(tmp[1 + 4 * i]) ^ xtime(tmp[2 + 4 * i]) ^ tmp[2 + 4 * i] ^ tmp[3 + 4 * i];//第二行
		s[2 + 4 * i] = tmp[4 * i] ^ tmp[1 + 4 * i] ^ xtime(tmp[2 + 4 * i]) ^ xtime(tmp[3 + 4 * i]) ^ tmp[3 + 4 * i];//第三行
		s[3 + 4 * i] = xtime(tmp[4 * i]) ^ tmp[ 4 * i] ^ tmp[1 + 4 * i] ^ tmp[2 + 4 * i] ^ xtime(tmp[3 + 4 * i]);//第四行
	}
}
//字节代换
void SubBytes(uint8_t* s) {
	for (int i = 0;i < 16;i++) {
		s[i] = Sbox[s[i]];
	}
}
void aes_encrypt(uint8_t *pt, uint8_t(*rk)[16], uint8_t *ct) {
	memcpy(ct, pt, 16);
	//异或白化密钥
	AddRoundKey(ct, rk[0]);
	int r = 1;
	//9轮核心轮函数f
	for (r; r <= 9;r++) {
		SubBytes(ct);
		ShiftRow(ct);
		MixColumns(ct);
		AddRoundKey(ct, rk[r]);
	}
	SubBytes(ct);
	ShiftRow(ct);
	AddRoundKey(ct, rk[r]);
}