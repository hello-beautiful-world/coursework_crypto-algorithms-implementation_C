#include"aes.h"
#include <stdint.h>
#include <string.h>
//逆行移位,循环右移
void InvShiftRow(uint8_t* s) {
	uint8_t tmp[16];//临时变量
	memcpy(tmp, s, 16);
	//第二行
	s[1] = tmp[13];
	s[5] = tmp[1];
	s[9] = tmp[5];
	s[13] = tmp[9];
	//第三行
	s[2] = tmp[10];
	s[6] = tmp[14];
	s[10] = tmp[2];
	s[14] = tmp[6];
	//第四行
	s[3] = tmp[7];
	s[7] = tmp[11];
	s[11] = tmp[15];
	s[15] = tmp[3];
}
//0E*uint_8[4]
uint8_t xtime_0E(uint8_t a) {
	return xtime(xtime(xtime(a))) ^ xtime(xtime(a)) ^ xtime(a);//08+04+02
}
//09*uint_8[4]
uint8_t xtime_09(uint8_t a) {
	return xtime(xtime(xtime(a))) ^ a;//08+01
}
//0B*uint_8[4]
uint8_t xtime_0B(uint8_t a) {
	return xtime_09(a) ^ xtime(a);//09+02
}
//0D*uint_8[4]
uint8_t xtime_0D(uint8_t a) {
	return xtime_09(a) ^ xtime(xtime(a));//09+04
}
//逆列混淆
void InvMixColumns(uint8_t* s) {
	uint8_t tmp[16];//临时变量
	memcpy(tmp, s, 16);
	for (int i = 0;i < 4;i++) {
		s[0 + 4 * i] = xtime_0E(tmp[4 * i]) ^ xtime_0B(tmp[1 + 4 * i]) ^ xtime_0D(tmp[2 + 4 * i]) ^ xtime_09(tmp[3 + 4 * i]);//第一行
		s[1 + 4 * i] = xtime_09(tmp[4 * i]) ^ xtime_0E(tmp[1 + 4 * i]) ^ xtime_0B(tmp[2 + 4 * i]) ^ xtime_0D(tmp[3 + 4 * i]);//第二行
		s[2 + 4 * i] = xtime_0D(tmp[4 * i]) ^ xtime_09(tmp[1 + 4 * i]) ^ xtime_0E(tmp[2 + 4 * i]) ^ xtime_0B(tmp[3 + 4 * i]);//第三行
		s[3 + 4 * i] = xtime_0B(tmp[4 * i]) ^ xtime_0D(tmp[1 + 4 * i]) ^ xtime_09(tmp[2 + 4 * i]) ^ xtime_0E(tmp[3 + 4 * i]);//第四行
	}
}
//逆字节代换
void InvSubBytes(uint8_t* s) {
	for (int i = 0;i < 16;i++) {
		s[i] = INV_Sbox[s[i]];
	}
}
//解密
void aes_decrypt(uint8_t* ct, uint8_t(*rk)[16], uint8_t* pt) {
	memcpy(pt, ct, 16);
	//异或密钥
	AddRoundKey(pt, rk[10]);//注意轮密钥使用顺序与加密相反
	int r = 1;
	//9轮核心轮函数f
	for (r; r <= 9;r++) {
		InvShiftRow(pt);
		InvSubBytes(pt);
		AddRoundKey(pt, rk[10-r]);
		InvMixColumns(pt);
	}
	InvShiftRow(pt);
	InvSubBytes(pt);
	AddRoundKey(pt, rk[0]);
}