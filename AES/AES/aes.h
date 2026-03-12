#ifndef AES_H
#define AES_H
#include <stdint.h>
//声明全局变量
extern uint8_t Sbox[];    //S盒，长度为256
extern uint8_t INV_Sbox[];//逆S盒，长度为256
//声明函数
void aes_key_schedule(uint8_t* key, uint8_t(*rk)[16]);//密钥扩展
void AddRoundKey(uint8_t* s, uint8_t* rk);//轮密钥加
void ShiftRow(uint8_t* s);//行移位
uint8_t xtime(uint8_t a);//02*uint8_t[4]
void MixColumns(uint8_t* s);//列混淆
void SubBytes(uint8_t* s);//字节代换
void aes_encrypt(uint8_t* pt, uint8_t(*rk)[16], uint8_t* ct);//加密
void InvShiftRow(uint8_t* s);//逆行移位,循环右移
uint8_t xtime_0E(uint8_t a);//0E*uint_8[4]
uint8_t xtime_09(uint8_t a);//09*uint_8[4]
uint8_t xtime_0B(uint8_t a);//0B*uint_8[4]
uint8_t xtime_0D(uint8_t a);//0D*uint_8[4]
void InvMixColumns(uint8_t* s);//逆列混淆
void InvSubBytes(uint8_t* s);//逆字节代换
void aes_decrypt(uint8_t* ct, uint8_t(*rk)[16], uint8_t* pt);//解密
#endif
