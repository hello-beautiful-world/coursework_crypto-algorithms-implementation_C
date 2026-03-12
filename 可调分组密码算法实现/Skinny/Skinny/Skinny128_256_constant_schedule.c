#include "Skinny128_256.h"
#include <stdint.h>
//6比特线性移位寄存器生成轮常量
void Skinny128_256_constant_schedule(uint8_t* constant) {
	uint8_t inva = 0x0;//初始值为全0，只有低6位有效
	for (int i = 0; i < SKINNY_ROUNDS; i++) {
		inva = ((inva << 1) ^ (1 ^ (inva >> 5) ^ ((inva >> 4) & 1))) & 0x3f;//更新寄存器,注意>>4后需要&1只取最低位,& 0x3f取低6位
		constant[i] = inva;
	}
}