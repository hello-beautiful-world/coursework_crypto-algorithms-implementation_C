#include "Ascon.h"
#include <stdint.h>
#include <stdlib.h>//malloc
#include <stdio.h>
#include <math.h>
int  Ascon_verified_decrypt(char* str_C, uint64_t * T, char* str_K, char* str_N, char* str_A, char* str_pt, int P_length) {
	//处理填充数据
	int t = (P_length + 15) / 16;//使用t个uint64_t存储密文，t=P_length*4/64上取整
	int s = (strlen(str_A) + 15) / 16;//使用s个uint64_t存储关联数据
	uint64_t* C = (uint64_t*)malloc(t * sizeof(uint64_t));//密文
	uint64_t* A = (uint64_t*)malloc(s * sizeof(uint64_t));//关联数据
	if (C == NULL || A == NULL) {
		printf("Memory allocation failed!\n");
		return; // 如果内存分配失败，返回
	}
	uint64_t K[2];//密钥,Ascon-128密钥长度为128，需要2个uint64_t数据存储
	uint64_t N[2];//公开随机数,长度为128，需要2个uint64_t数据存储
	//处理密文时，只需要保证前P_length*4bits密文是正确的即可，怎么填充都可以，后续再对解密得的明文处理，所以直接调用明文的填充方式
	process_data(str_C, t, C, str_K, K, str_N, N, str_A, s, A, P_length);

	//initialization
	uint64_t S[5];//初始320bits数据
	initialize(S, K, N);

	//Processing Associated Data
	for (int i = 0; i < s; i++) {
		S[0] ^= A[i];
		permute(S, 6);//b=6
	}
	S[4] ^= 0x01;

	//Processing Ciphertext
	uint64_t* pt = (uint64_t*)malloc(t * sizeof(uint64_t));//明文
	if (pt == NULL) {
		printf("Memory allocation failed!\n");
		return; // 如果内存分配失败，返回
	}
	for (int i = 0; i < t - 1; i++) {
		pt[i] = S[0] ^ C[i];//异或密文
		S[0] = C[i];//密文替换
		permute(S, 6);//b=6
	}

	pt[t - 1] = S[0] ^ C[t - 1];//异或密文
	int bit = (P_length * 4) % 64;
	if (bit == 0) S[0] ^= pt[t - 1];//明文为64的倍数,无需填充
	else {
		uint64_t tmp;//存储˜Pt || 1 || 0∗
		uint64_t mask = (~0ULL) << (63 - bit); // 创建掩码，~0ULL是64位全1，然后左移63-bit位
		tmp = (pt[t - 1] | (1ULL << (63 - bit))) & mask;//使用位或运算符|将第bit+1比特设置为1
		S[0] ^= tmp;
	}
	uint64_array_to_hex_string(pt, t, str_pt, P_length);//取出明文
	//Finalization
	S[1] ^= K[0];
	S[2] ^= K[1];
	permute(S, 12);//a=12
	uint64_t de_T[2];//验签,长度为128，需要2个uint64_t数据存储
	de_T[0] = S[3] ^ K[0];
	de_T[1] = S[4] ^ K[1];
	printf("\n验证得到的标签：\n");
	for (int i = 0; i < 2; i++) printf("%llx", de_T[i]);
	free(pt);
	free(A);
	free(C);
	if (T[0] == de_T[0] && T[1] == de_T[1]) return 1;
	else return 0;
}