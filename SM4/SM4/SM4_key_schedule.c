#include "SM4.h"
#include <stdint.h>
/*功能：字节替代
参数传递：X--32bit的字，每8bit过S盒
返回值：Y-每8bit过S盒后得到的32bit的字
*/
uint32_t SubByte(uint32_t X) {
	uint32_t Y = 0;
	for (int i = 0;i < 4;i++) Y ^= SBox[0xff & (X >> (i * 8))]<< (i * 8);
	return Y;
}
//密钥扩展的移位异或
uint32_t RotXor_key_schedule(uint32_t X) {
	return X ^ ROL(X, 13) ^ ROL(X, 23);
}
//密钥扩展算法
void SM4_key_schedule(uint32_t* key, uint32_t* rk) {
	uint32_t K[4];//临时变量，存储当前4个状态
	uint32_t K_4;//存储更新的状态
	int i = 0;
	for (i = 0; i < 4;i++) K[i] = key[i] ^ FK[i];//状态填充
	//32轮产生轮密钥
	for (i = 0;i < 32;i++) {
		K_4 = K[1] ^ K[2] ^ K[3] ^ CK[i];//分支异或
		K_4 = SubByte(K_4);//字节替代
		K_4 = RotXor_key_schedule(K_4);//移位异或
		K_4 ^= K[0];
		rk[i] = K_4;//保存第i+1轮的轮密钥
		//更新状态
		K[0] = K[1];
		K[1] = K[2];
		K[2] = K[3];
		K[3] = K_4;
	}
}