#include "Ascon.h"
#include <stdint.h>
//Ps-Substitution layer  with 5-bit S-box S(x)
void Ps(uint64_t* S) {
	S[0] ^= S[4];
	S[4] ^= S[3];
	S[2] ^= S[1];
	uint64_t t[5] = { ~S[0],~S[1],~S[2],~S[3],~S[4] };
	for (int i = 0; i < 5; i++) {
		t[i] &= S[(i + 1) % 5];
	}
	for (int i = 0; i < 5; i++) {
		S[i] ^= t[(i + 1) % 5];
	}
	S[1] ^= S[0];
	S[0] ^= S[4];
	S[3] ^= S[2];
	S[2] = ~S[2];
}
//PL-Linear layer with 64-bit diffusion functions
void PL(uint64_t* S) {
	S[0] = S[0] ^ ROR(S[0], 19) ^ ROR(S[0], 28);
	S[1] = S[1] ^ ROR(S[1], 61) ^ ROR(S[1], 39);
	S[2] = S[2] ^ ROR(S[2], 1) ^ ROR(S[2], 6);
	S[3] = S[3] ^ ROR(S[3], 10) ^ ROR(S[3], 17);
	S[4] = S[4] ^ ROR(S[4], 7) ^ ROR(S[4], 41);
}
/*功能：a次p置换（注意根据a不同，异或的轮常数不同）
参数传递：uint64_t S[5]*/
void permute(uint64_t* S, int a) {
	if (a == 6) {
		for (int r = 0; r < 6; r++) {
			S[2] ^= RC6[r];//Pc
			Ps(S);//Ps
			PL(S);//PL
		}
	}
	if (a == 12) {
		for (int r = 0; r < 12; r++) {
			S[2] ^= RC12[r];//Pc
			Ps(S);//Ps
			PL(S);//PL
		}
	}
}