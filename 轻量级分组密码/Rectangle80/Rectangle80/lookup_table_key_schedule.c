#include "Rectangle80_lookup_table.h"
#include<stdint.h>
//从80bit寄存器中取出第i轮的轮密钥,按比特取
void get_rk_bit(int* key, int(*rk)[64], int r) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 16; j++) {
				rk[r][16*i+j] = key[16*(3-i) + j];//注意此时rk[r][0]为最高位，便于后续查表异或
		}
	}
}
//密钥扩展算法
void Rectangle80_lookup_table_key_schedule(int* key, int* rk[][64]) {
	int K[80];
	//填充K=K_4∥K_3∥K_2∥K_1∥K_0,注意高低位
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 16; j++) {
			K[16 * i + j] = key[16 * (4 - i) + j];
		}
	}
	get_rk_bit(K, rk, 0);//取白化密钥
	//25次更新移位寄存器
	for (int r = 1; r < 26; r++) {
		S(K);//4个s盒变换
		row_ope(K);//行变换
		Xor_constant(K, r - 1);//异或轮常数
		get_rk_bit(K, rk, r);//取密钥
	}
}