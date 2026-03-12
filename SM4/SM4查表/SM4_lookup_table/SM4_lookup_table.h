#ifndef SM4_H
#define SM4_H
#include <stdint.h>
#define ROL(X, a) (uint32_t)((X<<a)|(X>>(32-a)))//循环左移a位
//声明全局变量
extern uint32_t FK[4];//系统参数4*32bit，用于密钥扩展
extern uint32_t CK[32];//密钥扩展算法的轮常量32*32bit
extern uint8_t SBox[256];//S盒,8bit->8bit,加密和密钥扩展使用相同S盒
extern uint32_t en_tabT[4][256] ;//加密T变换建表
extern uint32_t key_tabT[4][256];//密钥扩展T变换建表

//声明函数
uint32_t RotXor_key_schedule(uint32_t X);//密钥扩展的移位异或
void building_key_tabT();//创建密钥扩展T变换的四个表，表规模8bit->32bit
void SM4_key_schedule_lookup_table(uint32_t* key, uint32_t* rk);//密钥扩展算法
void uint8_to_uint32(uint8_t* X, uint32_t* Y);//将uint8_t X[16]转化位uint32_t Y[4]
void uint32_to_uint8(uint32_t* X, uint8_t* Y);//将uint32_t X[4]转化位uint8_t Y[16]
uint32_t RotXor(uint32_t X);//加密的移位异或
void building_en_tabT();//创建加密T变换的四个表，表规模8bit->32bit
void SM4_encrypt_lookup_table(uint8_t* pt, uint32_t* rk, uint8_t* ct);//加解密流程相同，轮密钥顺序相反
#endif
