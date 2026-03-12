#include "Ascon.h"
#include <stdint.h>//uint64_t
#include<stdio.h>
#include<string.h>//strlen
#include <stdlib.h>//malloc
int main() {
	char str_P[] = "000102030405060708090A0B0C0D0E0F10111213141516";//等待加密的任意长度的明文字符串
	char str_K[] = "000102030405060708090A0B0C0D0E0F";//key_length比特长度的密钥字符串
	char str_N[] = "000102030405060708090A0B0C0D0E0F";//128bits的公开随机数字符串
	char str_A[] = "000102030405060708";//任意长度的关联数据字符串
	uint64_t T[2];//认证标签，128bits
	int P_length = strlen(str_P);//明密文长度（比特长度=P_length*4）
	char* str_C = (char*)malloc(P_length * sizeof(char));//密文字符串，长度和str_P相同
	char* str_pt = (char*)malloc(P_length * sizeof(char));//解密获得的明文，与密文str_C长度相同
	int error;//标识是否认证成功，0表认证失败，1表成功
	printf("原明文:\n");
	printf("%s", str_P);
	//Ascon-128认证加密算法
	Ascon_authenticted_encrypt(str_P, str_K, str_N, str_A, str_C, T, P_length);
	printf("\n密文:\n");
	printf("%s", str_C);
	printf("\n认证标签：\n");
	for (int i = 0; i < 2; i++) printf("%llx", T[i]);
	//Ascon验证解密算法
	error = Ascon_verified_decrypt(str_C, T, str_K, str_N, str_A, str_pt, P_length);
	if (error) {
		printf("\n认证成功!");
		printf("\n解密所获得的明文：\n");
		printf("%s", str_pt);
	}
	else printf("\n认证失败！");
	free(str_C);
	free(str_pt);
	str_C = NULL;
	str_pt = NULL;
	return 0;
}