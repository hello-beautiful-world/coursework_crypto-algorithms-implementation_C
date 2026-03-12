#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#define BLOCK_SIZE 1048576      // 2^20
#define INT28_MAX 268435455     // 2^28-1

// PC-1表，用于密钥置换
static const uint8_t PC1_TABLE[56] = {
    57, 49, 41, 33, 25, 17, 9,
    1, 58, 50, 42, 34, 26, 18,
    10, 2, 59, 51, 43, 35, 27,
    19, 11, 3, 60, 52, 44, 36,
    63, 55, 47, 39, 31, 23, 15,
    7, 62, 54, 46, 38, 30, 22,
    14, 6, 61, 53, 45, 37, 29,
    21, 13, 5, 28, 20, 12, 4};

// 左移位数表
static const uint8_t LOOP_TABLE[16] = {
    1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1};

// PC-2表，用于生成子密钥
static const uint8_t PC2_TABLE[48] = {
    14, 17, 11, 24, 1, 5,
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32};

// 置换IP表
static const uint8_t IP_TABLE[64] = {
    58, 50, 42, 34, 26, 18, 10, 2,
    60, 52, 44, 36, 28, 20, 12, 4,
    62, 54, 46, 38, 30, 22, 14, 6,
    64, 56, 48, 40, 32, 24, 16, 8,
    57, 49, 41, 33, 25, 17, 9, 1,
    59, 51, 43, 35, 27, 19, 11, 3,
    61, 53, 45, 37, 29, 21, 13, 5,
    63, 55, 47, 39, 31, 23, 15, 7};

// 扩展运算表
static const uint8_t EXPANSION_TABLE[48] = {
    32, 1, 2, 3, 4, 5,
    4, 5, 6, 7, 8, 9,
    8, 9, 10, 11, 12, 13,
    12, 13, 14, 15, 16, 17,
    16, 17, 18, 19, 20, 21,
    20, 21, 22, 23, 24, 25,
    24, 25, 26, 27, 28, 29,
    28, 29, 30, 31, 32, 1};

// S盒
static const uint8_t S_BOX[8][4][16] = {
    // S1
    {
        {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7},
        {0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8},
        {4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0},
        {15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}},
    // S2
    {
        {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10},
        {3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5},
        {0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15},
        {13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}},
    // S3
    {
        {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8},
        {13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1},
        {13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7},
        {1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}},
    // S4
    {
        {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15},
        {13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9},
        {10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4},
        {3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}},
    // S5
    {
        {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9},
        {14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6},
        {4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14},
        {11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}},
    // S6
    {
        {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11},
        {10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8},
        {9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6},
        {4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}},
    // S7
    {
        {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1},
        {13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6},
        {1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2},
        {6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}},
    // S8
    {
        {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7},
        {1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2},
        {7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8},
        {2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11}}};

// P换位表
static const uint8_t P_TABLE[32] = {
    16, 7, 20, 21,
    29, 12, 28, 17,
    1, 15, 23, 26,
    5, 18, 31, 10,
    2, 8, 24, 14,
    32, 27, 3, 9,
    19, 13, 30, 6,
    22, 11, 4, 25};

/**
 * @brief DES 密钥生成
 * @param round_key 生成的加密轮密钥
 * @param seed_key  种子密钥
 */
void des_key_schedule(uint64_t round_key[][48], uint64_t seed_key);

/**
 * @brief DES 加密
 * @param plaintext     明文
 * @param round_key     轮密钥
 * @param ciphertext    加密得到的密文
 */
void des_encrypt(uint64_t plaintext[], uint64_t round_key[][48], uint64_t ciphertext[]);

/**
 * @brief DES 解密
 * @param ciphertext    密文
 * @param round_key     轮密钥
 * @param decryptext    解密得到的明文
 */
void des_decrypt(uint64_t ciphertext[], uint64_t round_key[][48], uint64_t decryptedtext[]);

int flag = 0;   // 标志加密还是解密

int main()
{
    // 明文
    uint64_t plaintext[64];
    // 密文
    uint64_t ciphertext[64];
    // 解密得到的明文
    uint64_t decryptedtext[64];
    // 种子密钥
    uint64_t seed_key = 0b1000000000100000000100000100100000000000000000100000000001000000ull;
    // 轮密钥
    uint64_t round_key[16][48];

    // 生成轮密钥
    des_key_schedule(round_key, seed_key);
    // for (int i = 0; i < 16; ++i) {
    //     for (int j = 0; j < 48; ++j)
    //         printf("%02llu", round_key[i][j]);
    //     printf("\n");
    // }

    uint64_t binary_num = 0b1000000001000000001000000001000000001000000001000000001000000001ull;
    // 使用一个二进制数给所有位赋值，简化操作
    for (int i = 0; i < 64; ++i) {
        // 检查 binary_num 的第 i 位是否为 1
        if ((binary_num >> 63 - i) & 1)
            plaintext[i] = UINT64_MAX; // 全 1
        else
            plaintext[i] = 0; // 全 0
    }

    printf("明文：\n");
    for (int i = 0; i < 64; ++i) {
        printf("%02llu ", plaintext[i]);
        if ((i + 1) % 8 == 0)
            printf("\n");
    }

    clock_t start = clock();
    // DES 加密
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        des_encrypt(plaintext, round_key, ciphertext);
    }
    clock_t end = clock();

    int data = 64 * 8 * BLOCK_SIZE / 1024 / 1024;
    double time = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("加密32MB的明文程序的运行时间为 %.4f 秒\n", time);//CLOCKS_PER_SEC是time.h定义的宏,CPU运行时钟周期数/s。
    printf("运行速度为 % .4f Mbps\n", 8 * data / time);
    // printf("密文：\n");
    // for (int i = 0; i < 64; ++i) {
    //     printf("%02llu ", ciphertext[i]);
    //     if ((i + 1) % 8 == 0)
    //         printf("\n");
    // }

    // DES 解密
    for (int i = 0; i < BLOCK_SIZE; ++i) {
        des_decrypt(ciphertext, round_key, decryptedtext);
    }

    printf("解密后明文：\n");
    for (int i = 0; i < 64; ++i) {
        printf("%02llu ", decryptedtext[i]);
        if ((i + 1) % 8 == 0)
            printf("\n");
    }

    return 0;
}

void des_key_schedule(uint64_t round_key[][48], uint64_t seed_key) {
    uint64_t temp = 0;

    // 置换选择 1
    for (int i = 0; i < 56; ++i)
        temp |= (seed_key >> (64 - PC1_TABLE[i]) & 1) << (55 - i);

    for (int i = 0; i < 16; ++i) {
        // 循环左移
        uint64_t left = temp >> 28, right = temp & INT28_MAX;
        left = (left << LOOP_TABLE[i] | left >> (28 - LOOP_TABLE[i])) & INT28_MAX;
        right = (right << LOOP_TABLE[i] | right >> (28 - LOOP_TABLE[i])) & INT28_MAX;
        temp = (left << 28) | right;

        // 置换选择 2
        for (int j = 0; j < 48; ++j) {
            if ((temp >> (56 - PC2_TABLE[j]) & 1) == 1)
                round_key[i][j] = UINT64_MAX;   // 赋值全1
            else
                round_key[i][j] = 0;            // 赋值全0
        }
    }
}

void des_encrypt(uint64_t plaintext[], uint64_t round_key[][48], uint64_t ciphertext[]) {
    uint64_t temp[64], *L = temp, *R = temp + 32, right[32];

    // 初始置换 IP
    for (int i = 0; i < 64; ++i)
        temp[i] = plaintext[IP_TABLE[i] - 1];

    // 轮函数
    for (int i = 0; i < 16; ++i) {
        memcpy(right, R, 32 * sizeof(uint64_t));

        // F 函数
        uint64_t startState[48], endState[32];

        // 选择扩展运算 E
        for (int j = 0; j < 48; ++j)
            startState[j] = R[EXPANSION_TABLE[j] - 1];
        
        // 子密钥异或
        if (!flag) {        // 加密
            for(int j = 0; j < 48; ++j)
                startState[j] ^= round_key[i][j];
        } else {            // 解密
            for(int j = 0; j < 48; ++j)
                startState[j] ^= round_key[15 - i][j];
        }
        
        // 选择压缩运算 S
        uint64_t *p = startState, *r = endState;
        for (int j = 0; j < 8; ++j, p += 6, r += 4) {
            // 用S1的布尔函数来替代其他所有S盒
            r[3] = (p[5] & p[4] & p[3] & p[2] & p[0]) ^ (p[5] & p[4] & p[2] & p[1] & p[0]) ^ (p[5] & p[4] & p[2] & p[1]) ^ (p[5] & p[4] & p[1] & p[0]) ^ (p[5] & p[4] & p[0]) ^ (p[5] & p[4]) ^ (p[5] & p[3] & p[2] & p[1] & p[0]) ^ (p[5] & p[3] & p[2] & p[0]) ^ (p[5] & p[3] & p[1]) ^ (p[5] & p[3] & p[0]) ^ (p[5] & p[2] & p[1]) ^ (p[5] & p[1]) ^ (p[5] & p[0]) ^ (p[4] & p[3] & p[2] & p[0]) ^ (p[4] & p[3] & p[1] & p[0]) ^ (p[4] & p[3] & p[1]) ^ (p[4] & p[3] & p[0]) ^ (p[4] & p[2] & p[0]) ^ (p[4] & p[2]) ^ (p[4] & p[1] & p[0]) ^ (p[4] & p[1]) ^ (p[4] & p[0]) ^ (p[3] & p[2] & p[1] & p[0]) ^ (p[3] & p[2] & p[0]) ^ (p[3] & p[0]) ^ p[3] ^ (p[2] & p[1] & p[0]) ^ (p[2] & p[0]) ^ p[1];
            r[2] = (p[5] & p[4] & p[3] & p[2] & p[0]) ^ (p[5] & p[4] & p[3] & p[1] & p[0]) ^ (p[5] & p[4] & p[3] & p[1]) ^ (p[5] & p[4] & p[3]) ^ (p[5] & p[4] & p[2] & p[1] & p[0]) ^ (p[5] & p[4] & p[1] & p[0]) ^ (p[5] & p[4] & p[0]) ^ (p[5] & p[3] & p[2] & p[1] & p[0]) ^ (p[5] & p[3] & p[2] & p[1]) ^ (p[5] & p[3] & p[2]) ^ (p[5] & p[3] & p[1] & p[0]) ^ (p[5] & p[3] & p[1]) ^ (p[5] & p[2] & p[1] & p[0]) ^ (p[5] & p[2] & p[1]) ^ (p[5] & p[2]) ^ (p[5] & p[1] & p[0]) ^ (p[5] & p[1]) ^ p[5] ^ (p[4] & p[3] & p[1] & p[0]) ^ (p[4] & p[3]) ^ (p[4] & p[2] & p[1] & p[0]) ^ (p[4] & p[2] & p[1]) ^ (p[4] & p[2]) ^ (p[4] & p[1]) ^ (p[4] & p[0]) ^ p[4] ^ (p[3] & p[2] & p[1] & p[0]) ^ (p[3] & p[2] & p[1]) ^ (p[3] & p[2] & p[0]) ^ (p[3] & p[2]) ^ (p[3] & p[1] & p[0]) ^ (p[3] & p[1]) ^ p[3] ^ (p[2] & p[1] & p[0]) ^ (p[2] & p[1]) ^ (p[1] & p[0]) ^ p[0] ^ INT64_MAX;
            r[1] = (p[5] & p[4] & p[3] & p[2] & p[0]) ^ (p[5] & p[4] & p[3] & p[2]) ^ (p[5] & p[4] & p[3] & p[1] & p[0]) ^ (p[5] & p[4] & p[2] & p[1] & p[0]) ^ (p[5] & p[4] & p[2] & p[0]) ^ (p[5] & p[4] & p[2]) ^ (p[5] & p[4]) ^ (p[5] & p[3] & p[2] & p[1] & p[0]) ^ (p[5] & p[3] & p[2]) ^ (p[5] & p[3] & p[1]) ^ (p[5] & p[3]) ^ (p[5] & p[2] & p[1] & p[0]) ^ (p[5] & p[2] & p[1]) ^ (p[5] & p[1] & p[0]) ^ (p[5] & p[1]) ^ (p[5] & p[0]) ^ p[5] ^ (p[4] & p[3] & p[1]) ^ (p[4] & p[3] & p[1]) ^ (p[4] & p[3] & p[0]) ^ (p[4] & p[3]) ^ (p[4] & p[2] & p[1] & p[0]) ^ (p[4] & p[2]) ^ (p[4] & p[1] & p[0]) ^ (p[4] & p[0]) ^ (p[3] & p[2] & p[1] & p[0]) ^ (p[3] & p[2] & p[0]) ^ (p[3] & p[1]) ^ (p[2] & p[1] & p[0]) ^ (p[2] & p[0]) ^ p[2] ^ (p[1] & p[0]) ^ p[1] ^ INT64_MAX;
            r[0] = (p[5] & p[4] & p[3]) ^ (p[5] & p[4] & p[2] & p[1] & p[0]) ^ (p[5] & p[4] & p[1] & p[0]) ^ (p[5] & p[3] & p[2] & p[1] & p[0]) ^ (p[5] & p[3] & p[2] & p[0]) ^ (p[5] & p[3] & p[2]) ^ (p[5] & p[3] & p[1] & p[0]) ^ (p[5] & p[3] & p[0]) ^ p[5] ^ (p[4] & p[3] & p[2] & p[0]) ^ (p[4] & p[3] & p[2]) ^ (p[4] & p[3] & p[1] & p[0]) ^ (p[4] & p[2] & p[0]) ^ (p[4] & p[0]) ^ p[4] ^ (p[3] & p[2] & p[1] & p[0]) ^ (p[3] & p[2] & p[1]) ^ (p[3] & p[2] & p[0]) ^ (p[3] & p[2]) ^ (p[3] & p[1] & p[0]) ^ (p[3] & p[0]) ^ (p[2] & p[1] & p[0]) ^ (p[2] & p[1]) ^ p[2] ^ p[1] ^ p[0] ^ INT64_MAX;
        }

        // 置换运算 P
        for (int j = 0; j < 32; ++j)
            R[j] = endState[P_TABLE[j] - 1];

        // 异或
        for (int j = 0; j < 32; ++j)
            R[j] ^= L[j];

        memcpy(L, right, 32 * sizeof(uint64_t));
    }
    memcpy(right, R, 32 * sizeof(uint64_t));
    memcpy(R, L, 32 * sizeof(uint64_t));
    memcpy(L, right, 32 * sizeof(uint64_t));

    // 初始逆置换 IP^-1
    for (int i = 0; i < 64; ++i)
        ciphertext[IP_TABLE[i] - 1] = temp[i];
    
    // 恢复标志位为加密状态
    flag = 0;
}

void des_decrypt(uint64_t ciphertext[], uint64_t round_key[][48], uint64_t decryptedtext[]) {
    flag = 1;   // 设置解密
    des_encrypt(ciphertext, round_key, decryptedtext);
}



//void left_shifts(uint64_t *array, int size, int shifts);
