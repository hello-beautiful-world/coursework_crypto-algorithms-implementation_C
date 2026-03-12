#ifndef RECTANGLE80_BIT_SLICE_H
#define RECTANGLE80_BIT_SLICE_H
#include <stdint.h>
#define ROL(X, a) (uint16_t)((X<<a)|(X>>(16-a)))//循环左移a位
//声明全局变量
extern uint8_t RC[25];//密钥扩展轮常数
extern int RC_bit[25][5];
//声明函数
void get_rk(int* key, uint16_t(*rk)[4], int r);//从80bit寄存器中取出第i轮的轮密钥
void S(int* key);//密钥扩展四个S盒变换
void row_ope(int* key);//密钥扩展的行变换
void Xor_constant(int* key, int r);//密钥扩展异或轮常数
void Rectangle80_key_schedule(int* key, uint16_t* rk[][4]);//密钥扩展算法
void en_S(uint16_t* ct);//加密比特切片过S盒
void Rectangle80_bit_slice_encrypt(uint16_t* pt, uint16_t(*rk)[4], uint16_t* ct);//加密
#endif
