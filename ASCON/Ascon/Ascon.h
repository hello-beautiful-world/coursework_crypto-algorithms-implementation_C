#ifndef ASCOM_H
#define ASCON_H
#include <stdint.h>
#define ROR(X, a) (uint64_t)((X<<(64-a))|(X>>a))//循环右移a位
//声明全局变量
extern int key_length;//密钥长度<=160，Ascon-128的密钥长度为128bits
uint64_t RC12[12]; //p-12的轮常数
uint64_t RC6[6]; //p-6的轮常数
//声明函数
//Ps-Substitution layer  with 5-bit S-box S(x)
void Ps(uint64_t* S);
//PL-Linear layer with 64-bit diffusion functions
void PL(uint64_t* S);
void permute(uint64_t* S, int a);
void fill_data(char* str_P, int t, uint64_t* P, int P_length);//功能：填充数据P为r - bit blocks of P || 1 || 0 *
void process_data(char* str_P, int t, uint64_t* P, char* str_K, uint64_t* K, char* str_N, uint64_t* N, char* str_A, int s, uint64_t* A, int P_length);//初始化，将相关数据填充完整
void initialize(uint64_t* S, uint64_t* K, uint64_t* N);
void uint64_array_to_hex_string(uint64_t* arr, int t, char* out, int str_length);//取uint64_t数组中前str_length*4bits元素放在字符串中
void Ascon_authenticted_encrypt(char* str_P, char* str_K, char* str_N, char* str_A, char* str_C, uint64_t* T, int P_length);//认证加密
int  Ascon_verified_decrypt(char* str_C, uint64_t* T, char* str_K, char* str_N, char* str_A, char* str_pt, int P_length);//验证解密
#endif