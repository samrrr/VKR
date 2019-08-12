#include "NCryptSyncDiffHelm.h"

inline void NCryptSyncDiffHelm::setp(bigint _p) {
	p = _p;
	havep = 1;
}

inline void NCryptSyncDiffHelm::setsz(int sz) {
	size = sz;
	numsz = (size + 63) / 64;
}

inline int NCryptSyncDiffHelm::step1(const vector<uint64_t>& arrin, vector<uint64_t>& arrout, int step, std::shared_ptr<NRandomGen> rg) {
	switch (step) {
	case 0:
	{
		if (!havep)
			p = randomExtraPrime(size, rg);
		do {
			g = randomNumberGen(rg, size + 10, 0) % p;
			std::stringstream ss;
			//ss<< "1:" << (powmod(g, 2, p)) << " 2:" << (powmod(g, (p - 1) / 2, p)) << " 3:" << (powmod(g, p - 1, p)) << std::endl;
			//std::cout << ss.str();

		} while (powmod(g, 2, p) == 1 || powmod(g, (p - 1) / 2, p) == 1);

		a = randomNumberGen(rg, size + 10, 0) % (p - 4) + 2;

		A = powmod(g, a, p);

		if (g.data().size() > numsz)
			return -1;
		if (p.data().size() > numsz)
			return -1;
		if (A.data().size() > numsz)
			return -1;
		size_t szg = g.data().size(), szp = p.data().size(), szA = A.data().size();

		arrout.resize(numsz * 3);
		std::fill(arrout.begin(), arrout.end(), 0);

		for (int i = 0; i < szg; i++) {
			arrout[i] = g.data()[i];
		}
		for (int i = 0; i < szp; i++) {
			arrout[i + numsz] = p.data()[i];
		}
		for (int i = 0; i < szA; i++) {
			arrout[i + numsz * 2] = A.data()[i];
		}

		return 0;
		break;
	}
	case 1:
	{
		vector<uint64_t> arr;

		if (arrin.size() != numsz)
			return -1;
		arr.resize(numsz);
		for (int i = 0; i < numsz; i++) {
			arr[i] = arrin[i];
		}
		B = bigint(arr, 1);

		K = powmod(B, a, p);

		arrout.resize(0);
		return 1;
		break;
	}
	}

	return -1;
}

inline int NCryptSyncDiffHelm::step2(const vector<uint64_t>& arrin, vector<uint64_t>& arrout, int step, std::shared_ptr<NRandomGen> rg) {
	switch (step) {
	case 0:
	{
		vector<uint64_t> arr;

		if (arrin.size() != numsz * 3)
			return -1;
		arr.resize(numsz);

		for (int i = 0; i < numsz; i++) {
			arr[i] = arrin[i];
		}
		g = bigint(arr, 1);

		for (int i = 0; i < numsz; i++) {
			arr[i] = arrin[i + numsz];
		}
		p = bigint(arr, 1);

		for (int i = 0; i < numsz; i++) {
			arr[i] = arrin[i + numsz * 2];
		}
		A = bigint(arr, 1);

		b = randomNumberGen(rg, size + 10, 0) % (p - 4) + 2;

		K = powmod(A, b, p);
		B = powmod(g, b, p);

		arrout.resize(numsz);
		for (int i = 0; i < B.data().size(); i++) {
			arrout[i] = B.data()[i];
		}

		return 1;
		break;
	}
	}

	return -1;
}

inline int NCryptSyncDiffHelm::stepsize(int type, int step) {
	switch (type) {
	case 1:
	{
		switch (step) {
		case 0:
		{
			return 0;
		}
		case 1:
		{
			return numsz;
		}
		default:
		{
			return -1;
		}
		}
	}
	case 2:
	{
		switch (step) {
		case 0:
		{
			return numsz * 3;
		}
		default:
		{
			return -1;
		}
		}
	}
	default:
	{
		return -1;
	}
	}
}

inline vector<uint64_t> NCryptSyncDiffHelm::getResult() {
	return K.data();
}

inline int NCryptSyncDiffHelm::randomBitsCount() {
	return size - 2;
}
