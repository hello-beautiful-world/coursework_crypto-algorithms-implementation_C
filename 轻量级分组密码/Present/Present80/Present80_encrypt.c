#include "Present80.h"
#include <stdint.h>
uint8_t Sbox[16] = { 0xc,0x5,0x6,0xb,0x9,0x0,0xa,0xd,0x3,0xe,0xf,0x8,0x4,0x7,0x1,0x2 };
uint8_t new_S[256];//两个S盒合并为一个新S盒
uint64_t Tab[8][256] = {0};//S和P层之后的结果表,一次查表过两次S盒
//将数据经过S和P层之后的结果，通过预计算做成表
void building_Tab() {
	uint8_t y, b;
	//合并2个S盒
	for (int x = 0; x < 16; x++) {
		y = Sbox[x];
		for (int a = 0; a < 16; a++) {
			b = Sbox[a];
			new_S[(x << 4) | a] = (y << 4) | b;
		}
	}
	//建表，循环8次
	uint8_t tmp;//用于临时存放过S层(8bit)后的结果
	for (int i = 0; i < 8; i++) {
		for(int j = 0; j < 256; j++) {
			//S变换
			tmp = new_S[j];
			//P变换
			Tab[i][j] ^= (((uint64_t)(tmp >> 7)) << (63-2 * i)) | (((uint64_t)((tmp >> 6)&0x01)) << (47-2 * i)) | (((uint64_t)((tmp >> 5) & 0x01)) << (31-2 * i)) | (((uint64_t)((tmp >> 4) & 0x01)) << (15-2 * i));
			Tab[i][j] ^= (((uint64_t)((tmp >> 3) & 0x01)) << (62-2 * i)) | (((uint64_t)((tmp >> 2) & 0x01)) << (46-2 * i)) | (((uint64_t)((tmp >> 1) & 0x01)) << (30-2 * i)) | (((uint64_t)(tmp & 0x01)) << (14-2 * i));//太长代码不好看，就分两次对8bit异或
		}
	}
}
//x经过SP变换，查表实现
uint64_t SP(uint64_t x) {
	uint64_t y = 0;
	for (int i = 0; i < 8; i++) {
		y ^= Tab[i][(x >> (56 - 8 * i)) & 0xff];
	}
	return y;
}
//将uint8_t X[8]转化为uint68_t Y
void uint8_to_uint64(uint8_t* X, uint64_t* Y) {
	for (int i = 0; i < 8; i++) {
		*Y ^= (((uint64_t)X[i]) << (56 - 8 * i));
	}
}
//将uint68_t Y转化为uint8_t X[8]
void uint64_to_uint8(uint8_t* X, uint64_t Y) {
	for (int i = 0; i < 8; i++) {
		X[i] = (Y >> (56 - 8 * i)) & 0xff;
	}
}
//加密
void Present80_encrypt(uint8_t* pt, uint64_t* rk, uint8_t* ct) {
	uint64_t x = 0;
	uint8_to_uint64(pt, &x);//将pt装填至x
	//31轮
	for (int r = 0; r < 31; r++) {
		x ^= rk[r];//异或轮密钥
		x = SP(x);//SP变换，使用查表实现
	}
	x ^= rk[31];//异或白化密钥
	uint64_to_uint8(ct, x);//将x装填至ct
}