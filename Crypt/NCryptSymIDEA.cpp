#include "NCryptSymIDEA.h"

const uint64_t NCryptSymIDEA::GetDataScale()
{
	return 8;
}

const uint64_t NCryptSymIDEA::GetKeySize()
{
	return 128/8;
}

uint16_t opxor(uint16_t a, uint16_t b) {
	return a ^ b;
}
uint16_t opadd(uint16_t a, uint16_t b) {
	return a + b;
}
uint16_t opmul(uint32_t a, uint32_t b) {
	if (a == 0)
		a += 0x10000;
	if (b == 0)
		b += 0x10000;
	return (a * b) % 0x10001;
}

void gcdex(int a, int b, int &x, int &y) {

	int p = 1, q = 0, r = 0, s = 1;

	while (a && b) {
		if (a >= b) {
			a = a - b;
			p = p - r;
			q = q - s;
		}
		else
		{
			b = b - a;
			r = r - p;
			s = s - q;
		}
	}
	if (a) {
		x = p;
		y = q;
	}
	else
	{
		x = r;
		y = s;
	}
}

uint16_t opinv(uint16_t a) {
	if (a == 0)
		return 0;
	int x, y;
	gcdex(a, 0x10001, x, y);
	return (x+0x10001) %0x10001;
}

bool NCryptSymIDEA::SetKey(std::vector<uint64_t> v)
{
	if(v.size()!=128 / 64)
		return false;
	haveK2 = 0;

	uint16_t *ilink=reinterpret_cast<uint16_t*>(v.data());
	

	v[0] =
		((v[0] & 0x000000000000ffff) << 48) |
		((v[0] & 0x00000000ffff0000) << 16) |
		((v[0] & 0x0000ffff00000000) >> 16) |
		((v[0] & 0xffff000000000000) >> 48);
	v[1] =
		((v[1] & 0x000000000000ffff) << 48) |
		((v[1] & 0x00000000ffff0000) << 16) |
		((v[1] & 0x0000ffff00000000) >> 16) |
		((v[1] & 0xffff000000000000) >> 48);

	for (int i = 0; i < 6; i++) {
		K[i * 8 + 0] = ilink[3];
		K[i * 8 + 1] = ilink[2];
		K[i * 8 + 2] = ilink[1];
		K[i * 8 + 3] = ilink[0];
		K[i * 8 + 4] = ilink[7];
		K[i * 8 + 5] = ilink[6];
		K[i * 8 + 6] = ilink[5];
		K[i * 8 + 7] = ilink[4];
		
		uint64_t k1, k2;
		k1 = (v[0] << 25) | (v[1] >> (64 - 25));
		k2 = (v[1] << 25) | (v[0] >> (64 - 25));
		v[0] = k1;
		v[1] = k2;
	}

	K[6 * 8 + 0] = ilink[3];
	K[6 * 8 + 1] = ilink[2];
	K[6 * 8 + 2] = ilink[1];
	K[6 * 8 + 3] = ilink[0];

	return true;
}


uint64_t IDEAEncode(uint64_t src,uint16_t *K) {
	uint16_t D0 = src;
	uint16_t D1 = src >> 16;
	uint16_t D2 = src >> 32;
	uint16_t D3 = src >> 48;

	for (int i = 0; i < 8; i++) {
		uint16_t p01, p02, p03, p04;
		uint16_t p11, p12;
		uint16_t p21, p22;
		uint16_t p31, p32;
		uint16_t 
			K1 = K [i*6+0], 
			K2 = K[i * 6 + 1], 
			K3 = K[i * 6 + 2], 
			K4 = K[i * 6 + 3],
			K5 = K[i * 6 + 4],
			K6 = K[i * 6 + 5];
		
		p01 = opmul(D0, K1);
		p02 = opadd(D1, K2);
		p03 = opadd(D2, K3);
		p04 = opmul(D3, K4);

		p11 = opxor(p01, p03);
		p12 = opxor(p02, p04);

		p21 = opmul(p11, K5);
		p22 = opadd(p12, p21);

		p32 = opmul(p22, K6);
		p31 = opadd(p32, p21);

		D0 = opxor(p01, p32);
		D1 = opxor(p03, p32);
		D2 = opxor(p02, p31);
		D3 = opxor(p04, p31);
		

	}

	D0 = opmul(D0, K[8 * 6 + 0]);
	D1 = opadd(D1, K[8 * 6 + 2]);
	D2 = opadd(D2, K[8 * 6 + 1]);
	D3 = opmul(D3, K[8 * 6 + 3]);

	return D0 | uint64_t(D2) << 16 | uint64_t(D1) << 32 | uint64_t(D3) << 48;

}

bool NCryptSymIDEA::Encode(void *src, void *dst, uint64_t sz)
{
	if(sz%(64/8) !=0)
		return false;

	for(int i=0;i<sz/8;i++)
		reinterpret_cast<uint64_t*>(dst)[i]=IDEAEncode(reinterpret_cast<uint64_t*>(src)[i],K);

	return true;
}

bool NCryptSymIDEA::Decode(void *src, void *dst, uint64_t sz)
{
	if (sz % (64 / 8) != 0)
		return false;

	if (!haveK2)
		genK2();

	for (int i = 0; i<sz / 8; i++)
		reinterpret_cast<uint64_t*>(dst)[i] = IDEAEncode(reinterpret_cast<uint64_t*>(src)[i], K2);

	return false;
}

NCryptSymIDEA::~NCryptSymIDEA()
{
}

void NCryptSymIDEA::genK2()
{
	haveK2 = 1;
	
	for (int i = 0, r = 6*8; i < 52; i++,r++) {
		switch (i%6)
		{
		case 0:
			K2[r] = opinv(K[i]);
			break;
		case 1:
			K2[r] = -K[i + 1];
			break;
		case 2:
			K2[r] = -K[i - 1];
			break;
		case 3:
			K2[r] = opinv(K[i]);
			r -= 6;
			break;
		case 4:
			K2[r] = K[i];
			break;
		case 5:
			K2[r] = K[i];
			r -= 6;
			break;
		}
	}
	std::swap(K2[1], K2[2]);
	std::swap(K2[1 + 8 * 6], K2[2 + 8 * 6]);
}



