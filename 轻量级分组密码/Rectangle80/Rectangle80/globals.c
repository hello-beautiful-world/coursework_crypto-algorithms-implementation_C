#include "Rectangle80_lookup_table.h"
#include <stdint.h>
//密钥扩展轮常数
uint8_t RC[25] = { 
	0X01,0X02,0X04,0X09,0X12,0X05,
	0X0B,0X16,0X0C,0X19,0X13,0X07,
	0X0F,0X1F,0X1E,0X1C,0X18,0X11,
	0X03,0X06,0X0D,0X1B,0X17,0X0E,
	0X1D };
int RC_bit[25][5];
int Sbox[16] = { 6,5,12,10,1,14,7,9,11,0,3,13,8,15,4,2 };
int Sbox_bit[16][4]; //将S盒转化为bit存储
int SPT[16][16][64] = { 0 };//建表4bit,初始化为全0
int SPT_2[8][256][64] = { 0 };//建表8bit,初始化为全0
//将uint8_t类型轮常量转化成25*5bit
void RC_transf() {
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 5; j++) {
			RC_bit[i][j] = ((RC[i] >> (4 - j)) & 1) == 1 ? 1 : 0;
		}
	}
}
/*比特切片
#include "Rectangle80_bit_slice.h"
#include <stdint.h>
//密钥扩展轮常数
uint8_t RC[25] = {
	0X01,0X02,0X04,0X09,0X12,0X05,
	0X0B,0X16,0X0C,0X19,0X13,0X07,
	0X0F,0X1F,0X1E,0X1C,0X18,0X11,
	0X03,0X06,0X0D,0X1B,0X17,0X0E,
	0X1D };
int RC_bit[25][5];
//将uint8_t类型轮常量转化成25*5bit
void RC_transf() {
	for (int i = 0; i < 25; i++) {
		for (int j = 0; j < 5; j++) {
			RC_bit[i][j] = ((RC[i] >> (4 - j)) & 1) == 1 ? 1 : 0;
		}
	}
}
*/