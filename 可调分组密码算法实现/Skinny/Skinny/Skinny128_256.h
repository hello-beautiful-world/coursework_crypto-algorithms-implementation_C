#ifndef SKINNY_H
#define SKINNY_H
#include <stdint.h>
#define SKINNY_ROUNDS 48//Skinny128/256轮数为48
//声明全局变量

extern int	PT[16];//可调密钥编排算法的置换表
extern uint8_t Sbox[256];//S盒
extern uint8_t inv_Sbox[256];//逆S盒
extern uint8_t constant[SKINNY_ROUNDS];//轮常量寄存器
//声明函数

//6比特线性移位寄存器生成轮常量
void Skinny128_256_constant_schedule(uint8_t* constant);
//PT字置换操作：
void permutate(uint8_t* key);
//LFSR操作,作用于前两行的每一个字
void LFSR(uint8_t* key);
//可调密钥编排算法
void Skinny128_256_tweakey_schedule(uint8_t(*key)[16], uint8_t rk[SKINNY_ROUNDS][2][8]);
//创建S盒
void building_Sbox();
//Skinny128_256加密
void Skinny128_256_encrypt(uint8_t* pt, uint8_t rk[SKINNY_ROUNDS][2][8], uint8_t* ct);
//Skinny128_256解密
void Skinny128_256_decrypt(uint8_t* ct, uint8_t rk[SKINNY_ROUNDS][2][8], uint8_t* de_pt);
#endif