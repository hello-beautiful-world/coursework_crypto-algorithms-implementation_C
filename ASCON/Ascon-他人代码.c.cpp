#include<stdio.h>
#include<stdint.h>
# define ROL(X,a) (uint64_t) (X>>a)^(X<<(64-a)) //定义循环右移的宏 


uint64_t S[5];
uint64_t IV = 0x80400c0600000000;
//轮常量
uint8_t Cr12[12] = {
	0xf0,0xe1,0xd2,0xc3,0xb4,0xa5,0x96,0x87,0x78,0x69,0x5a,0x4b
};

uint8_t Cr6[6] = {
	0x96,0x87,0x78,0x69,0x5a,0x4b
};
//Pa
void permutation_a() {
	for (int r = 0; r < 12; r++) {
		//PC
		S[2] ^= Cr12[r];//第三层异或轮常量
		//PS（过S盒）
		uint64_t T[5];
		S[0] ^= S[4]; S[4] ^= S[3]; S[2] ^= S[1];
		T[0] = S[0];  T[1] = S[1];  T[2] = S[2];  T[3] = S[3];  T[4] = S[4];
		T[0] = ~T[0];  T[1] = ~T[1];  T[2] = ~T[2];  T[3] = ~T[3];  T[4] = ~T[4];
		T[0] &= S[1]; T[1] &= S[2]; T[2] &= S[3]; T[3] &= S[4]; T[4] &= S[0];
		S[0] ^= T[1]; S[1] ^= T[2]; S[2] ^= T[3]; S[3] ^= T[4]; S[4] ^= T[0];
		S[1] ^= S[0]; S[0] ^= S[4]; S[3] ^= S[2]; S[2] = ~S[2];
		//PL（线性层）
		S[0] = S[0] ^ ROL(S[0], 19) ^ ROL(S[0], 28);
		S[1] = S[1] ^ ROL(S[1], 61) ^ ROL(S[1], 39);
		S[2] = S[2] ^ ROL(S[2], 1) ^ ROL(S[2], 6);
		S[3] = S[3] ^ ROL(S[3], 10) ^ ROL(S[3], 17);
		S[4] = S[4] ^ ROL(S[4], 7) ^ ROL(S[4], 41);
	}
}

//Pb
void permutation_b() {
	for (int r = 0; r < 6; r++) {
		//PC
		S[2] ^= Cr6[r];
		//PS
		uint64_t T[5];
		S[0] ^= S[4]; S[4] ^= S[3]; S[2] ^= S[1];
		T[0] = S[0];  T[1] = S[1];  T[2] = S[2];  T[3] = S[3];  T[4] = S[4];
		T[0] = ~T[0];  T[1] = ~T[1];  T[2] = ~T[2];  T[3] = ~T[3];  T[4] = ~T[4];
		T[0] &= S[1]; T[1] &= S[2]; T[2] &= S[3]; T[3] &= S[4]; T[4] &= S[0];
		S[0] ^= T[1]; S[1] ^= T[2]; S[2] ^= T[3]; S[3] ^= T[4]; S[4] ^= T[0];
		S[1] ^= S[0]; S[0] ^= S[4]; S[3] ^= S[2]; S[2] = ~S[2];
		//PL
		S[0] = S[0] ^ ROL(S[0], 19) ^ ROL(S[0], 28);
		S[1] = S[1] ^ ROL(S[1], 61) ^ ROL(S[1], 39);
		S[2] = S[2] ^ ROL(S[2], 1) ^ ROL(S[2], 6);
		S[3] = S[3] ^ ROL(S[3], 10) ^ ROL(S[3], 17);
		S[4] = S[4] ^ ROL(S[4], 7) ^ ROL(S[4], 41);
	}
}

uint64_t K[2] = { 0 };
//初始化阶段 S ← IV||K||N
void Init_Mode(uint8_t key[], uint64_t N[2]) {
	//装载数据
	for (int i = 0; i < 8; i++) {
		K[0] ^= ((uint64_t)key[i]) << (56 - 8 * i);
		K[1] ^= ((uint64_t)key[i + 8]) << (56 - 8 * i);
	}
	//S ← IV||K||N
	S[0] = IV; S[1] = K[0]; S[2] = K[1]; S[3] = N[0]; S[4] = N[1];
	//S ← Pa(s)^((0^(320-k))||K)
	permutation_a();
	//异或 
	S[3] ^= K[0]; S[4] ^= K[1];
}

//关联数据阶段 
void Associated_Data_Mode(uint8_t AD[], const int ADLength) {
	int i, j, r;
	uint64_t A;
	int Length = ADLength / 8; //AD的组数（64bits为一组）
	int num = ADLength % 8;//最后一组的字节数
	//处理除的尽的部分
	for (r = 0; r < Length; r++) {
		//与AD异或 
		A = 0x0;
		for (j = 0; j < 8; j++)
			A ^= ((uint64_t)AD[8 * r + j]) << (56 - 8 * j);
		S[0] ^= A;
		//Permutation_b 
		permutation_b();
	}
	//补空位
	if (num != 0) {
		A = 0x0;
		for (j = 0; j < num; j++)
			A ^= ((uint64_t)AD[8 * Length + j]) << (56 - 8 * j);
		A ^= ((uint64_t)0x80) << (56 - 8 * j);//填充1，0
		S[0] ^= A;
		permutation_b();
	}
	S[4] ^= 0x01;
}

//加密阶段 
void Encrypt_Mode(uint8_t pt[], uint8_t ct[], int ptLength) {
	int i, j, r;
	uint64_t C;
	int Length = ptLength / 8; //pt的组数（64bits为一组）  31/8=3
	const int num = ptLength % 8;//最后一组的字节数  31%8=7
	if (num != 0)
		Length += 1;
	for (r = 0; r < Length - 1; r++) {
		C = 0x0;
		for (i = 0; i < 8; i++) {
			C ^= ((uint64_t)pt[8 * r + i]) << (56 - 8 * i);
		}
		C^=S[0];
		S[0] = C;
		//获得密文
		for (i = 0; i < 8; i++) {
			ct[8 * r + i] = (S[0] >> (56 - 8 * i)) & 0xff;
		}
		//Permutation
		permutation_b();
	}
	//最后一组加密 
	if (num != 0) {
		C = 0x0;
		for (j = 0; j < num; j++) {
			C ^= ((uint64_t)pt[8 * (Length - 1) + j]) << (56 - 8 * j);
		}
		C ^= ((uint64_t)0x80) << (56 - 8 * j);
	}
	else {
		C = 0x0;
		for (int i = 0; i < 8; i++) {
			C ^= ((uint64_t)pt[8 * (Length - 1) + i]) << (56 - 8 * i);
		}
	}
	C ^= S[0];
	S[0] = C;
	//获得密文
	for (i = 0; i < num; i++) {
		ct[8 * (Length - 1) + i] = (S[0] >> (56 - 8 * i)) & 0xff;
	}
}

//解密阶段
void Decrypt_Mode(uint8_t ct[], uint8_t pt[], int ctlength)
{
	int i, j, r;
	uint64_t C;
	int length = ctlength / 8;
	const int num = ctlength % 8;
	if (num != 0)
		length += 1;
	for (r = 0; r < length - 1; r++)
	{
		C = 0x0;
		for (j = 0; j < 8; j++)
		{
			C ^= ((uint64_t)ct[8 * r + j]) << (56 - 8 * j);
		}
		C ^= S[0];
		S[0] = C;
		permutation_b();
	}

	C = 0x0;
	for (i = 0; i < 8; i++)
	{
		C ^= ((uint64_t)pt[8 * (length - 1) + i]) << (56 - 8 * i);
	}
	C ^= S[0];
	C ^= ((uint64_t)0x80) << (56 - 8 * j);
	S[0] = C;
	//获得明文
	for (int i = 0; i < num; i++) {
		ct[8 * (length - 1) + i] = (S[0] >> (56 - 8 * i)) & 0xff;
	}
}

//生成Tag 
void Finalization_Mode(uint64_t T[2]) {
	S[1] ^= K[0]; S[2] ^= K[1];
	//pa
	permutation_a();
	T[0] = S[3] ^ K[0]; T[1] = S[4] ^ K[1];
}
void Ascon_Encrypt(uint8_t key[], uint64_t N[2], uint8_t AD[], const int ADLength, uint8_t pt[], uint8_t ct[], int ptLength, uint64_t T[2]) {
	Init_Mode(key, N);
	Associated_Data_Mode(AD, ADLength);
	Encrypt_Mode(pt, ct, ptLength);
	Finalization_Mode(T);
}

void Ascon_Decrypt(uint8_t key[], uint64_t N[2], uint8_t AD[], const int ADLength, uint8_t pt[], uint8_t ct[], int ctLength, uint64_t T1[2]) {
	Init_Mode(key, N);
	Associated_Data_Mode(AD, ADLength);
	Decrypt_Mode(ct, pt, ctLength);
	Finalization_Mode(T1);
}

int main() {
	int i;
	uint64_t T[2];
	uint64_t T1[2];
	uint64_t N[2] = { 0x0001020304050607,0x08090A0B0C0D0E0F };
	uint8_t AD[] = {
		0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08
	};
	uint8_t key[] = {
			0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F
	};
	uint8_t pt[] = {
		0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0A,0x0B,0x0C,0x0D,0x0E,0x0F,
		0x10,0x11,0x12,0x13,0x14,0x15,0x16
	};
	const int ADLength = sizeof(AD);
	const int ptLength = sizeof(pt);
	uint8_t ct[ptLength];
	Ascon_Encrypt(key, N, AD, ADLength, pt, ct, ptLength, T);
	printf("ciphertext:\n");
	for (int i = 0; i < ptLength; i++)
		printf("%x ", ct[i]);
	printf("\nTag:%llx %llx\n", T[0], T[1]);
	const int ctLength = sizeof(ct);
	Ascon_Decrypt(key, N, AD, ADLength, pt, ct, ctLength, T1);
	printf("plaintext:\n");
	for (int i = 0; i < ctLength; i++)
		printf("%x ", pt[i]);
	printf("\nTag:%llx %llx\n", T1[0], T1[1]);
	return 0;
}
