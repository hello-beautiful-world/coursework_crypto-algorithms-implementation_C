#include"aes_lookup_table.h"
#include <stdint.h>
#include <string.h>
uint32_t Inv_T[4][256] = { 0 };//逆MC表，4*8bit-->4*32bit
uint32_t Inv_TS[4][256] = { 0 };//逆SB、MC表，4*8bit-->4*32bit
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
//逆字节代换
void InvSubBytes(uint8_t* s) {
	for (int i = 0;i < 16;i++) {
		s[i] = INV_Sbox[s[i]];
	}
}
//创建逆MC表，4*8bit-->4*32bit
void Inv_T_build_tabla() {
	for (int i = 0;i < 256;i++) {
		Inv_T[0][i] = (((uint32_t)xtime_0B(i)) << 24) ^ (((uint32_t)xtime_0D(i)) << 16) ^ (((uint32_t)xtime_09(i)) << 8) ^ ((uint32_t)xtime_0E(i));//0Bx,0Dx,09x,0Ex(矩阵第一列倒序),注意排在列上方的放右边
		Inv_T[1][i] = (Inv_T[0][i] << 8) ^ (Inv_T[0][i] >> 24);
		Inv_T[2][i] = (Inv_T[1][i] << 8) ^ (Inv_T[1][i] >> 24);
		Inv_T[3][i] = (Inv_T[2][i] << 8) ^ (Inv_T[2][i] >> 24);
	}
}
//创建逆SB、MC表，4*8bit-->4*32bit
void Inv_TS_build_tabla() {
	for (int j = 0; j < 4;j++) {
		for (int i = 0;i < 256;i++) {
			Inv_TS[j][i] = Inv_T[j][INV_Sbox[i]];
		}
	}
}
//4*32bit-->16*8bit,4个转换位16个字节
void word4_to_byte16(uint8_t* byte, uint32_t* word) {
	for (int i = 0;i < 4;i++) {
		byte[4 * i] = word[i] & 0xff;//取最后8位
		byte[1 + 4 * i] = (word[i] >> 8) & 0xff;
		byte[2 + 4 * i] = (word[i] >> 16) & 0xff;
		byte[3 + 4 * i] = word[i] >> 24;
	}
}
//解密
void aes_decrypt(uint8_t* ct, uint8_t(*rk)[16], uint8_t* pt) {
	memcpy(pt, ct, 16);
	uint32_t tmp[4] = { 0 };//临时变量，4*32bit
	uint8_t rk_tmp[16] = { 0 };//临时变量，16*8bit
	//异或密钥
	AddRoundKey(pt, rk[10]);//注意轮密钥使用顺序与加密相反
	int r = 1;
	//9轮核心轮函数f
	for (r; r <= 9;r++) {
	    //实现SR、SB、MC
		for (int i = 0;i < 4;i++) {
			tmp[i] = Inv_TS[0][pt[(4 * i) % 16]] ^ Inv_TS[1][pt[(13 + 4 * i) % 16]] ^ Inv_TS[2][pt[(10 + 4 * i) % 16]] ^ Inv_TS[3][pt[(7 + 4 * i) % 16]];//同时实现行移位
		}
		//4*32bit-->16*8bit
		word4_to_byte16( pt, tmp);
		//轮密钥加
		for (int i = 0;i < 4;i++){
			tmp[i] = Inv_T[0][rk[10 - r][4*i]] ^ Inv_T[1][rk[10 - r][1+ 4 * i]] ^ Inv_T[2][rk[10 - r][2+ 4 * i]] ^ Inv_T[3][rk[10 - r][3+ 4 * i]];
		}
		//4*32bit-->16*8bit
		word4_to_byte16( rk_tmp, tmp);
		AddRoundKey(pt, rk_tmp);
	}
	InvShiftRow(pt);
	InvSubBytes(pt);
	AddRoundKey(pt, rk[0]);
}