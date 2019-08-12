#include "NCryptAuthRSA.h"

int NCryptAuthRSA::MaxMSize() {
	return key_n.data().size();
}

bool NCryptAuthRSA::SetPrivate(std::vector<uint64_t> PrivateKey) {
	if (PrivateKey.size() < 4)
		return false;

	uint64_t n1 = PrivateKey[0];

	if (PrivateKey.size() < n1 + 1)
		return false;
	vector<uint64_t> arr1(n1);
	for (int i = 0; i < n1; i++) {
		arr1[i] = PrivateKey[i + 1];
	}


	if (PrivateKey.size() < n1 + 2)
		return false;
	uint64_t n2 = PrivateKey[n1 + 1];

	if (PrivateKey.size() < n1 + 1 + n2 + 1)
		return false;
	vector<uint64_t> arr2(n2);
	for (int i = 0; i < n2; i++) {
		arr2[i] = PrivateKey[i + n1 + 2];
	}

	key_d = bigint(arr1, 1);
	key_n = bigint(arr2, 1);

	return true;
}

bool NCryptAuthRSA::SetToken(std::vector<uint64_t> PublicKey) {
	if (PublicKey.size() < 4)
		return false;

	uint64_t n1 = PublicKey[0];

	if (PublicKey.size() < n1 + 1)
		return false;
	vector<uint64_t> arr1(n1);
	for (int i = 0; i < n1; i++) {
		arr1[i] = PublicKey[i + 1];
	}


	if (PublicKey.size() < n1 + 2)
		return false;
	uint64_t n2 = PublicKey[n1 + 1];

	if (PublicKey.size() < n1 + 1 + n2 + 1)
		return false;
	vector<uint64_t> arr2(n2);
	for (int i = 0; i < n2; i++) {
		arr2[i] = PublicKey[i + n1 + 2];
	}

	key_e = bigint(arr1, 1);
	key_n = bigint(arr2, 1);

	return true;
}

bool NCryptAuthRSA::Test(std::vector<uint64_t> arrm, std::vector<uint64_t> arrh) {
	bigint h(arrh, 1);
	h = h % key_n;
	bigint m(arrm, 1);
	if (key_n == 0)
		return false;
	m = powmod(m, key_e, key_n);
	return m == h;
}

bool NCryptAuthRSA::Generate(int64_t size, std::vector<uint64_t>& PrivateKey, std::vector<uint64_t>& PublicKey, std::shared_ptr<NRandomGen> rg) {
	bigint p1, p2, n, eiler_n, e, d;
	p1 = randomPrime(static_cast<int>(size), rg);
	p2 = randomPrime(static_cast<int>(size), rg);
	n = p1 * p2;
	eiler_n = (p1 - 1) * (p2 - 1);

	vector<uint64_t> arr;
	arr.resize(eiler_n.data().size());
	bigint gx, gy;
	bigint gres = 0;
	while (gres != 1) {

		rg->GetRandom(arr.data(), arr.size() * sizeof(uint64_t));
		e = bigint(arr, 1);
		e = e % eiler_n;
		if (e < (bigint(1) << static_cast<int>(size / 2))) {
			e = e + (bigint(1) << static_cast<int>(size / 2));
		}

		gres = gcdex(e, eiler_n, gx, gy);
	}
	d = (gx + eiler_n) % eiler_n;
	auto &narr = n.data();
	auto &earr = e.data();
	auto &darr = d.data();


	bigint m = 2;
	m = powmod(m, e, n);
	m = powmod(m, d, n);
	if (m != 2)
		throw;


	PrivateKey.resize(darr.size() + narr.size() + 2);

	PrivateKey[0] = darr.size();
	for (int i = 0; i<darr.size(); i++)
		PrivateKey[i + 1] = darr[i];

	PrivateKey[darr.size() + 1] = narr.size();
	for (int i = 0; i<narr.size(); i++)
		PrivateKey[i + darr.size() + 2] = narr[i];



	PublicKey.resize(earr.size() + narr.size() + 2);

	PublicKey[0] = earr.size();
	for (int i = 0; i<earr.size(); i++)
		PublicKey[i + 1] = earr[i];

	PublicKey[earr.size() + 1] = narr.size();
	for (int i = 0; i<narr.size(); i++)
		PublicKey[i + earr.size() + 2] = narr[i];

	return true;
}

bool NCryptAuthRSA::Crypt(std::vector<uint64_t> harr, std::vector<uint64_t>& res) {

	bigint h = bigint(harr, 1);

	h = h % key_n;
	h = powmod(h, key_d, key_n);

	res = h.data();

	return true;
}

NCryptAuthRSA::~NCryptAuthRSA() {

}
