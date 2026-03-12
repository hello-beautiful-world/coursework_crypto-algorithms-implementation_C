#include "Rectangle80_lookup_table.h"
#include <stdint.h>
#include <string.h>
//拉线，取每Xi对应位置的比特元素依次排放
void P0(int* X) {
	int tmp[64];
	for (int i = 0; i < 64; i++) {
		int num_16 = i / 16;
		int k = i % 16;
		tmp[4 * k + num_16] = X[i];
	}
	memcpy(X, tmp, sizeof(tmp));
}
//逆拉线
void inv_P0(int* X){
	int tmp[64];
	for (int i = 0; i < 64; i++) {
		int num_4 = i / 4;
		int k = i % 4;
		tmp[16 * k + num_4] = X[i];
	}
	memcpy(X, tmp, sizeof(tmp));
}
//SPT[16][16][64]，分为16组（4比特一组，16个S盒），4比特有16种，得到64比特的结果
void build_tables() {
	for (int i = 0; i < 16; i++) {//将S盒转化为bit存储
		for (int j = 0; j < 4; j++) {
			Sbox_bit[i][j] = ((Sbox[i] >> (3 - j)) & 1);//Sbox_bit[i][0]为高位
		}
	}
	for (int i = 0; i < 16; i++) {//过第i个S盒遍历
		for (int j = 0; j < 16; j++) {//4比特的值为j
			//过一个S盒，逆拉线
			SPT[i][j][i] = Sbox_bit[j][0];
			SPT[i][j][i+16] = Sbox_bit[j][1];
			SPT[i][j][i+32] = Sbox_bit[j][2];
			SPT[i][j][i+48] = Sbox_bit[j][3];
			//循环左移
				//循环左移13位
			if (i < 13) SPT[i][j][3 + i] = SPT[i][j][i];
			else SPT[i][j][i - 13] = SPT[i][j][i];
			SPT[i][j][i] = 0;
				//循环左移12位
			if (i < 12) SPT[i][j][20 + i] = SPT[i][j][i + 16];
			else SPT[i][j][4 + i] = SPT[i][j][i + 16];//16+i-12=4+i
			SPT[i][j][i+16] = 0;
				//循环左移1位
			if (i == 0) SPT[i][j][47] = SPT[i][j][32];
			else SPT[i][j][31 + i] = SPT[i][j][32 + i];
			SPT[i][j][i + 32] = 0;
			//拉线
			P0(SPT[i][j]);
		}
	}
}
//创建8bit查表
void build_tables_8bit() {
	build_tables();
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 256; j++) {
			int fir4bit = (j >> 4);//取高4比特的值
			int sec4bit = (j & 15);//取低4比特的值
			int tmp[64] = { 0 };
			//合并
			for (int k = 0; k < 64; k++) {
				tmp[k] = SPT[2 * i][fir4bit][k] ^ SPT[2 * i + 1][sec4bit][k];
			}
			memcpy(SPT_2[i][j], tmp, 64 * sizeof(int));
		}
	}
}