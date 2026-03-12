#include "Simon128_128.h"
#include <stdint.h>
void Simon_128_128_key_schedule(uint64_t* key, uint64_t* rk) {
	uint64_t c = 0xfffffffffffffffc;//轮常量
	rk[0] = key[0];
	rk[1] = key[1];
	char z_2[] = "10101111011100000011010010011000101000010001111110010110110011";//轮常量
	for (int i = 2; i < SIMON_ROUNDS; i++) {
		rk[i] = ROR(rk[i - 1], 3) ^ ROR(rk[i - 1], 4) ^ rk[i - 2] ^ c ^ (z_2[(i - 2) % 62] == '0' ? 0 : 1);
	}
}