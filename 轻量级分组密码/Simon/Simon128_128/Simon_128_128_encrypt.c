#include "Simon128_128.h"
#include <stdint.h>
//将uint8_t x[16]转化为uint64_t y[2]
void uint8_to_uint64( uint64_t* y, uint8_t* x) {
	y[0] = 0;
	y[1] = 0;
	for (int i = 0; i < 8; i++) {
		y[0] ^= (uint64_t)(x[i]) << (64 - (i + 1) * 8);//注意轮操作前左右交叉变换
		y[1] ^= (uint64_t)(x[i+8]) << (64 - (i + 1) * 8);
	}
}
//将uint64_t y[2]转化为uint8_t x[16]
void uint64_to_uint8(uint8_t* x, uint64_t* y) {
	for (int i = 0; i < 8; i++) {
		x[i+8] = (y[0] >> (64 - (i + 1) * 8)) & 0xff;//注意最后一轮左右不交叉变换
		x[i] = (y[1] >> (64 - (i + 1) * 8)) & 0xff;
	}
}
//加密
void Simon_128_128_encrypt(uint8_t* pt, uint64_t* rk, uint8_t* ct) {
	uint64_t x[2];
	uint64_t tmp;//临时变量
	uint8_to_uint64(x, pt);
	for (int r = 0; r < SIMON_ROUNDS; r++) {
		tmp = x[0];
		x[0] = x[1] ^ (ROL(x[0], 1) & ROL(x[0], 8)) ^ ROL(x[0], 2) ^ rk[r];
		x[1] = tmp;
	}
	uint64_to_uint8(ct, x);
}