#ifndef SIMON_H
#define SIMON_H
#include <stdint.h>
#define SIMON_ROUNDS 68//Simon128/128轮数为68
#define n 64
#define m 2
#define ROL(X, a) (uint64_t)((X<<a)|(X>>(64-a)))//循环左移a位
#define ROR(X, a) (uint64_t)((X>>a)|(X<<(64-a)))//循环右移a位
//声明函数
void Simon_128_128_key_schedule(uint64_t* key, uint64_t* rk);//密钥扩展
void uint8_to_uint64(uint64_t* y, uint8_t* x);//将uint8_t x[16]转化为uint64_t y[2]
void uint64_to_uint8(uint8_t* x, uint64_t* y);//将uint64_t y[2]转化为uint8_t x[16]
void Simon_128_128_encrypt(uint8_t* pt, uint64_t* rk, uint8_t* ct);//加密
#endif
