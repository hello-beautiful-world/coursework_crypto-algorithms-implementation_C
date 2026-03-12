#include "Skinny128_256.h"
#include <stdint.h>
//可调密钥编排算法的置换表
int	PT[16] = { 9, 15, 8, 13, 10, 14, 12, 11, 0, 1, 2, 3, 4, 5, 6, 7 };
//S盒
uint8_t Sbox[256];
//逆S盒
uint8_t inv_Sbox[256];
//轮常量寄存器
uint8_t constant[SKINNY_ROUNDS];