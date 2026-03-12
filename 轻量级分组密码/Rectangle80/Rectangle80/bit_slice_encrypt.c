#include "Rectangle80_bit_slice.h"
#include <stdint.h>
#include<string.h>
//加密比特切片过S盒
void en_S(uint16_t* ct) {
	uint16_t X[4];
	memcpy(X, ct, sizeof(ct));
	ct[0] = X[0] ^ (X[0] & X[1]) ^ X[2] ^ X[3];
	ct[1] = 0xffff ^ X[0] ^ X[1] ^ X[2] ^ (X[1] & X[3]);
	ct[2] = 0xffff ^ (X[0] & X[1]) ^ X[2] ^ (X[0] & X[2]) ^ (X[1] & X[2]) ^ (X[0] & X[1] & X[2]) ^ X[3] ^ (X[2] & X[3]);
	ct[3] = X[1] ^ (X[0] & X[2]) ^ (X[1] & X[2]) ^ X[3] ^ (X[0] & X[3]) ^ (X[1] & X[2] & X[3]);
}
void Rectangle80_bit_slice_encrypt(uint16_t* pt, uint16_t(* rk)[4], uint16_t* ct) {
	int r;
	memcpy(ct, pt, sizeof(pt));
	//25轮
	for (r = 0; r < 25; r++) {
		//异或轮密钥
		for (int i = 0; i < 4; i++) {
			ct[i] ^= rk[r][i];
		}
		en_S(ct);//S层
		//P层
		ct[1] = ROL(ct[1], 1);//注意移位结果为ROL的返回值，ct本身值不变
		ct[2] = ROL(ct[2], 12);
		ct[3] = ROL(ct[3], 13);
	}
	for (int i = 0; i < 4; i++) {
		ct[i] ^=rk[r][i];
	}
}