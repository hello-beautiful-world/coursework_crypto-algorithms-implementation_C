#include"aes.h"
#include <stdint.h>
#include <string.h>
//void RotWord()
uint8_t Rcon[10] = { 0x01, 0x02, 0x04, 0x08, 0x10,
					 0x20, 0x40, 0x80, 0x1B, 0x36 }; //轮常数
void aes_key_schedule(uint8_t *key, uint8_t (*rk)[16]) {
	memcpy(rk[0], key, 16);//初始密钥复制,注意这里的 key 是一个指针（uint8_t *key），在32位系统上 sizeof(key) 通常为4字节，在64位系统上通常为8字节，而不是16字节（key 所指向的数组的实际大小）。
	for (int r = 1; r <= 10; r++) {
		//循环上移一个字节
		rk[r][0] = rk[r - 1][13];
		rk[r][1] = rk[r - 1][14];
		rk[r][2] = rk[r - 1][15];
		rk[r][3] = rk[r - 1][12];
		//S盒变换,异或轮常量,异或前一轮密钥的对应字节
		for (int i = 0;i < 4;i++) {
			if(i==0)
				rk[r][i] = Sbox[rk[r][i]] ^ Rcon[r - 1] ^ rk[r - 1][i];//注意轮常量为1字节，只需每一列最上面的字节异或轮常量
			else
				rk[r][i] = Sbox[rk[r][i]]  ^ rk[r - 1][i];
		}
		//计算后续密钥
		for (int i = 1; i <= 3;i++) {
			for (int j = 0;j < 4;j++) rk[r][4*i+j] =  rk[r][4*(i-1)+j]^rk[r-1][4*i+j];
		}
	}
}