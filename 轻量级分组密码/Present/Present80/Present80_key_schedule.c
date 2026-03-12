#include "Present80.h"
#include <stdint.h>
//r0r1循环左移61位 
void ROL_61(uint64_t* r0, uint16_t* r1) {
	uint64_t new_r0 = ((((uint64_t)0x07ULL) & *r0) << 61) | (((uint64_t)*r1) << 45) | (*r0 >> 19);//注意对应位置,具体细节看实验报告
	*r1 = (*r0 >> 3) & 0xffff;
	*r0 = new_r0;
}
//使用S盒代数表达式,r0的高4位过S盒 
void S(uint64_t *r0) {
	uint8_t X = (*r0 >> 60) & 0xff;//取出高4位
	uint8_t x[] = { X & 0x01,(X & 0x02) >> 1,(X & 0x04) >> 2,(X & 0x08) >> 3 };//取对应数值x3\x2\x1\x1放在uint8_t的最后一位 
	uint8_t y3 = 0x01 ^ x[0] ^ x[1] ^ x[3] ^ (x[1] & x[2]) ^ (x[0] & x[1] & x[2]) ^ (x[0] & x[1] & x[3]) ^ (x[0] & x[2] & x[3]);
	uint8_t y2 = 0x01 ^ x[2] ^ x[3] ^ (x[0] & x[1]) ^ (x[0] & x[3]) ^ (x[1] & x[3]) ^ (x[0] & x[1] & x[3]) ^ (x[0] & x[2] & x[3]);
	uint8_t y1 = x[1] ^ x[3] ^ (x[1] & x[3]) ^ (x[2] & x[3]) ^ (x[0] & x[1] & x[2]) ^ (x[0] & x[1] & x[3]) ^ (x[0] & x[2] & x[3]);
	uint8_t y0 = x[0] ^ x[2] ^ x[3] ^ (x[1] & x[2]);//y3/y2/y1/y0均只使用最后1bit
	*r0 = (((uint64_t)((y3 << 3) ^ (y2 << 2) ^ (y1 << 1) ^ y0)) << 60) ^ (*r0 &0x0fffffffffffffff);
}
//更新寄存器第三步，将(κ_19,κ_18,κ_17,κ_16,κ_15)与常数i+1进行异或。
void updateRegister_3(uint64_t* r0, uint16_t* r1, uint16_t iadd1) {
	*r0 ^= ((uint64_t)(iadd1 >> 1));
	*r1 ^= (iadd1 << 15);
}
void Present80_key_schedule(uint64_t key_0, uint16_t key_1, uint64_t* rk) {
	uint16_t iadd1;//i+1,用于更新寄存器第三步，低5位有效
	rk[0] = key_0;
	uint64_t r0 = key_0;//寄存器第一部分，64bit
	uint16_t r1 = key_1;//寄存器第二部分，16bit
	//更新寄存器
	for (int r = 0; r < 31; r++) {
		ROL_61(&r0, &r1);//循环左移61位
		S(&r0);//对最高四位比特做S盒变换
		iadd1 = r + 1;
		updateRegister_3(&r0, &r1, iadd1);//将(κ_19,κ_18,κ_17,κ_16,κ_15)与常数i+1进行异或
		rk[r + 1] = r0;//取寄存器的高64位作为轮密钥
	}
}