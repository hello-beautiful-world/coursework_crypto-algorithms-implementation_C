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
void SubBytes(uint8_t* s);//字节代换
uint8_t xtime_03(uint8_t a);//03*uint_8[4]
void MC_build_tabla();//列混淆建表
void SB_MC_build_tabla();//字节代换、列混淆建表
void aes_encrypt(uint8_t* pt, uint8_t(*rk)[16], uint8_t* ct);//加密
void InvShiftRow(uint8_t* s);//逆行移位,循环右移
uint8_t xtime_0E(uint8_t a);//0E*uint_8[4]
uint8_t xtime_09(uint8_t a);//09*uint_8[4]
uint8_t xtime_0B(uint8_t a);//0B*uint_8[4]
uint8_t xtime_0D(uint8_t a);//0D*uint_8[4]
void InvSubBytes(uint8_t* s);//逆字节代换
void Inv_T_build_tabla();//创建逆MC表，4*8bit-->4*32bit
void Inv_TS_build_tabla();//创建逆SB、MC表，4*8bit-->4*32bit
void word4_to_byte16(uint8_t* word, uint32_t* byte);//4*32bit-->16*8bit,4个转换位16个字节
void aes_decrypt(uint8_t* ct, uint8_t(*rk)[16], uint8_t* pt);//解密
#endif
