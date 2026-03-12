#include "Ascon.h"
#include <stdint.h>
#include <stdlib.h>//malloc
#include <string.h>//strca
#include <stdio.h>
/*
功能：填充数据P为r - bit blocks of P || 1 || 0 *
参数传递：t-用来表示str_P的uint64_t元素个数，P_length-16进制表示str_P的长度
*/
void fill_data(char* str_P, int t, uint64_t* P, int P_length) {
	int t16 = t * 16;//填充后用t16个4bits表示
	char* filled_str_p = (char*)malloc(t16 * sizeof(char)+1);//填充后的字符串，长度是64bits的倍数
	// 检查 malloc 是否成功
	if (filled_str_p == NULL) {
		printf("Memory allocation failed!\n");
		return; // 如果内存分配失败，返回
	}
	memcpy(filled_str_p, str_P, P_length*sizeof(char));//复制原文
	for (int i = 0; i < t16 - P_length; i++) {//填充
		if (i == 0) filled_str_p[P_length] = '8';//填充上1000
		else filled_str_p[P_length + i] = '0';//填充全0
	}
	for (int i = 0; i < t; i++) {//转换为uint64_t类型
		sscanf_s(filled_str_p + 16 * i, "%16llx", P + i);
	}
	free(filled_str_p);
}
//初始化，将相关数据填充完整
void process_data(char* str_P, int t, uint64_t* P, char* str_K, uint64_t* K, char* str_N, uint64_t* N, char* str_A, int s, uint64_t* A, int P_length) {
	//处理明文
	fill_data(str_P, t, P, P_length);
	//printf("\n填充后明文P:");
	//for (int i = 0; i < t; i++) printf("%llx  ", P[i]);
	//处理密钥
	sscanf_s(str_K, "%16llx", K);
	sscanf_s(str_K + 16, "%16llx", K + 1);
	//printf("\n密钥K:");
	//for (int i = 0; i < 2; i++) printf("%llx  ", K[i]);
	//处理随机数据
	sscanf_s(str_N, "%16llx", N);
	sscanf_s(str_N + 16, "%16llx", N + 1);
	//printf("\n随机数N：");
	//for (int i = 0; i < 2; i++) printf("%llx  ", N[i]);
	//处理关联数据
	fill_data(str_A, s, A, strlen(str_A));
	//printf("\n填充关联数据A:");
	//for (int i = 0; i < s; i++) printf("%llx  ", A[i]);
}
void initialize(uint64_t* S, uint64_t* K, uint64_t* N) {
	//S ← IVk,r,a,b || K || N
	char IV[] = "80400c0600000000";//64bits
	sscanf_s(IV, "%16llx", S);
	S[1] = K[0];
	S[2] = K[1];
	S[3] = N[0];
	S[4] = N[1];
	//printf("\nIVk,r,a,b || K || N:");
	//for (int i = 0; i < 5; i++) printf("%llx  ", S[i]);
	//S ← pa(S) ⊕K
	permute(S, 12);//a=12
	S[3] ^= K[0];
	S[4] ^= K[1];
}
/*
功能：取uint64_t数组中前str_length*4bits元素放在字符串中
参数传递：t-用来表示uint64_t元素个数，P_length-16进制表示原文的长度
*/
void uint64_array_to_hex_string(uint64_t* arr, int t, char* out, int str_length) {
	char* hex_string = (char*)malloc((t * 16 + 1) * sizeof(char));//// 每个64位整数转换的16个字符 + 空字符
	if (hex_string == NULL) {  // 如果内存分配失败
		fprintf(stderr, "Memory allocation failed\n");
		return;  // 退出函数，避免使用未分配的内存
	}
	hex_string[0] = '\0';//注意初始化 hex_string，否则strcat 尝试在一个未初始化的内存区域追加字符串，造成未定义行为。
	// 使用 snprintf 格式化每个元素，逐个连接
	out[0] = '\0';  // 确保输出字符串初始化为空
	for (size_t i = 0; i < t; ++i) {
		char temp[20];  // 用来存储每个64位整数的十六进制表示
		snprintf(temp, sizeof(temp), "%016llx", arr[i]);  // 格式化为16位十六进制，前面补零
		strcat_s(hex_string, 16 * t + 1, temp);  // 将格式化后的字符串追加到输出字符串中
	}
	for (int i = 0; i < str_length; i++) {
		out[i] = hex_string[i];
	}
	out[str_length] = '\0';
	free(hex_string);
}
void Ascon_authenticted_encrypt(char* str_P, char* str_K, char* str_N, char* str_A, char* str_C, uint64_t* T, int P_length) {
	//处理填充数据
	int t = (P_length + 15) / 16;//使用t个uint64_t存储明文，t=P_length*4/64上取整
	int s = (strlen(str_A) + 15) / 16;//使用s个uint64_t存储关联数据
	uint64_t* P = (uint64_t*)malloc(t * sizeof(uint64_t));//明文
	uint64_t* A = (uint64_t*)malloc(s * sizeof(uint64_t));//关联数据
	if (P == NULL || A == NULL) {
		printf("Memory allocation failed!\n");
		return; // 如果内存分配失败，返回
	}
	uint64_t K[2];//密钥,Ascon-128密钥长度为128，需要2个uint64_t数据存储
	uint64_t N[2];//公开随机数,长度为128，需要2个uint64_t数据存储
	process_data(str_P, t, P, str_K, K, str_N, N,str_A, s, A, P_length);

	//initialization
	uint64_t S[5];//初始320bits数据
	initialize(S, K, N);

	//Processing Associated Data
	for (int i = 0; i < s; i++) {
		S[0] ^= A[i];
		permute(S, 6);//b=6
	}
	S[4] ^= 0x01;

	//Processing Plaintext
	uint64_t* C = (uint64_t*)malloc(t * sizeof(uint64_t));//密文
	if (C == NULL) {
		printf("Memory allocation failed!\n");
		return; // 如果内存分配失败，返回
	}
	for (int i = 0; i < t - 1; i++) {
		S[0] ^= P[i];
		C[i] = S[0];
		permute(S, 6);//b=6
	}
	S[0] ^= P[t-1];
	C[t - 1] = S[0];
	uint64_array_to_hex_string(C, t, str_C, P_length);//取出密文
	//Finalization
	S[1] ^= K[0];
	S[2] ^= K[1];
	permute(S, 12);//a=12
	T[0] = S[3] ^ K[0];
	T[1] = S[4] ^ K[1];
	free(P);
	free(A);
	free(C);
	P = NULL;
	A = NULL;
	C = NULL;
}