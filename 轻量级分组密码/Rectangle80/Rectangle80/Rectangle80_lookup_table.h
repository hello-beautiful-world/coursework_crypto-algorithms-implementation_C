#ifndef RECTANGLE80_LOOKUP_TABLE_H
#define RECTANGLE80_LOOKUP_TABLE_H
#include <stdint.h>
#define ROL(X, a) (uint16_t)((X<<a)|(X>>(16-a)))//循环左移a位
//声明全局变量
extern uint8_t RC[25];//密钥扩展轮常数
extern int RC_bit[25][5];
extern int Sbox[16];
extern int SPT[16][16][64];
extern int Sbox_bit[16][4]; //将S盒转化为bit存储
extern int SPT_2[8][256][64];//建表8bit,初始化为全0
//声明函数
void RC_transf();//将uint8_t类型轮常量转化成25*5bit
void P0(int* X);//拉线，取每Xi对应位置的比特元素依次排放
void inv_P0(int* X);//逆拉线
void build_tables();//SPT[16][16][64]，分为16组（4比特一组，16个S盒），4比特有16种，得到64比特的结果
void build_tables_8bit();//SPT[8][256][64],创建8bit查表
void get_rk_bit(int* key, int(*rk)[64], int r);//从80bit寄存器中取出第i轮的轮密钥,按比特取
void Rectangle80_lookup_table_key_schedule(int* key, int* rk[][64]);//密钥扩展
void Xor_rk(int* ct, int(*rk)[64], int r);//一轮异或轮密钥
void Rectangle80_lookup_table_encrypt(int* pt, int(*rk)[64], int* ct);
#endif