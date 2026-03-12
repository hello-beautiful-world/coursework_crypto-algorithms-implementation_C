#ifndef PRESENT80_H
#define PRESENT80_H
#include <stdint.h>
#define ROL(X, a) (uint64_t)((X<<a)|(X>>(64-a)))//循环左移a位
//声明全局变量
extern uint64_t Tab[8][256];//S和P层之后的结果表,一次查表过两次S盒
//声明函数
void ROL_61(uint64_t* r0, uint16_t* r1);//r0r1循环左移61位 
void S(uint64_t* r0);//使用S盒代数表达式,r0的高4位过S盒 
void updateRegister_3(uint64_t* r0, uint16_t* r1, uint16_t iadd1);//更新寄存器第三步，将(κ_19,κ_18,κ_17,κ_16,κ_15)与常数i+1进行异或。
void Present80_key_schedule(uint64_t key_0, uint16_t key_1, uint64_t* rk);
void building_Tab();//将数据经过S和P层之后的结果，通过预计算做成表
uint64_t SP(uint64_t x);//x经过SP变换，查表实现
void uint8_to_uint64(uint8_t* X, uint64_t* Y);//将uint8_t X[8]转化为uint68_t Y
void uint64_to_uint8(uint8_t* X, uint64_t Y);//将uint68_t Y转化为uint8_t X[8]
void Present80_encrypt(uint8_t* pt, uint64_t* rk, uint8_t* ct);//加密
#endif
