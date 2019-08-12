#include "NCryptHashSHA3.h"

uint64_t rot(uint64_t x, int n)
{
	n = n % 64;
	return (((x << n) | (x >> (64 - n))));
}

class FState
{
public:
	uint64_t A[25];
	size_t sz, newsz;
	FState(int SZ) {
		if (SZ % 8 != 0)
			throw;

		memset(A, 0, 25 * sizeof(uint64_t));
		newsz = (1600 - 2 * SZ) / 8;
		sz = SZ;
	}
	void adddata(uint64_t *buf) {
		for (unsigned int i = 0; i < newsz / 8; i++)
		{
			A[i] ^= buf[i];
		}
	}
};

const uint64_t RC[] = {
	0x0000000000000001L, 0x0000000000008082L, 0x800000000000808aL,
	0x8000000080008000L, 0x000000000000808bL, 0x0000000080000001L,
	0x8000000080008081L, 0x8000000000008009L, 0x000000000000008aL,
	0x0000000000000088L, 0x0000000080008009L, 0x000000008000000aL,
	0x000000008000808bL, 0x800000000000008bL, 0x8000000000008089L,
	0x8000000000008003L, 0x8000000000008002L, 0x8000000000000080L,
	0x000000000000800aL, 0x800000008000000aL, 0x8000000080008081L,
	0x8000000000008080L, 0x0000000080000001L, 0x8000000080008008L
};

const int R[] = {
	0, 1, 62, 28, 27, 36, 44, 6, 55, 20, 3, 10, 43,
	25, 39, 41, 45, 15, 21, 8, 18, 2, 61, 56, 14
};

uint64_t rotl(uint64_t x, int n)
{
	return (x << n) | (x >> (64 - n));
}

uint64_t rotr(uint64_t x, int n)
{
	return (x >> n) | (x << (64 - n));
}

void keccakf(FState &s) {

	uint64_t B[25];
	uint64_t C[5];
	uint64_t D[5];

	uint64_t *A = s.A;
	for (int n = 0; n < 24; n++) {

		//theta
		for (int i = 0; i < 5; i++)
			C[i] = A[i + 5 * 0] ^ A[i + 5 * 1] ^ A[i + 5 * 2] ^ A[i + 5 * 3] ^ A[i + 5 * 4];

		for (int i = 0; i < 5; i++)
			D[i] = C[(i + 4) % 5] ^ rotl(C[(i + 1) % 5], 1);

		for (int i = 0; i < 5; i++)
			for (int r = 0; r < 5; r++)
				A[i + r * 5] ^= D[i];

		//rho pi
		for (int x = 0; x<5; x++)
			for (int y = 0; y<5; y++)
				B[y + ((2 * x + 3 * y) % 5) * 5] = rotl(A[x + y * 5], R[x + y * 5]);
		//chi
		for (int x = 0; x<5; x++)
			for (int y = 0; y<5; y++)
				A[x + y * 5] = B[x + y * 5] ^ ((~B[(x + 1) % 5 + y * 5]) & B[(x + 2) % 5 + y * 5]);

		//ota
		A[0] ^= RC[n];

	}

}
void HashSHA3(void *src, uint64_t srcsz, vector<uint64_t> &des) {
	FState st(512);
	vector<uint8_t> buf(st.newsz, 0);
	//0x06 0x80

	for (int i = 0; i < srcsz / st.newsz; i++) {
		st.adddata(static_cast<uint64_t*>(src) + i * st.newsz / 8);
		keccakf(st);
	}
	int mm = srcsz % st.newsz;
	//if (mm != 0) 
	{
		int offs = srcsz / st.newsz* st.newsz;
		for (int i = 0; i < st.newsz; i++)
			buf[i] = 0;
		for (int i = 0; i < mm; i++)
			buf[i] = *(static_cast<uint8_t*>(src) + offs + i);
		buf[mm] |= 0x06;
		buf[st.newsz - 1] |= 0x80;

		st.adddata((uint64_t*)(buf.data()));

		keccakf(st);

	}

	des.resize(512 / 64);

	for (int i = 0; i < des.size(); i++)
		des[i] = st.A[i];

}

bool NCryptHashSHA3::Hash(void *src, uint64_t srcsz, std::vector<uint64_t>&des)
{
	HashSHA3(src, srcsz, des);
	return true;
}

int NCryptHashSHA3::HashSize()
{
	return 512/64;
}

NCryptHashSHA3::~NCryptHashSHA3()
{
}
