#include "NCryptChannel.h"

inline NCryptChannel::NCryptChannel(NCryptSTR & s) {
	crypt = s;
}

inline bool NCryptChannel::Connect(std::string ip, int port) {
	bool b;
	b = crypt.connect->connectToServer(ip.c_str(), port);
	std::cout << "CH conn:" << b << std::endl;
	if (!b)
		return 0;
	myType = 1;
	return 1;
}

inline bool NCryptChannel::Listen(int port) {
	bool b;
	b = crypt.connect->listenClients(port);
	std::cout << "CH listen:" << b << std::endl;
	if (!b)
		return 0;
	myType = 2;
	return 1;
}

inline bool NCryptChannel::synckey() {
	int res;

	vector<uint64_t> keyres;
	int randBits = crypt.sync->randomBitsCount();
	if (randBits == 0)
		return 0;
	int currKeyBits = 0;
	int keyReady = 0;
	int keyNeedSize = crypt.symm->GetKeySize() * 8;
	keyres.resize((crypt.symm->GetKeySize() + 7) / 8);

	do {

		vector<uint64_t> vec, vec1;

		int st = 0;

		if (myType == 1) {
			std::cout << "CH synckey cl" << std::endl;
			do {
				vec.resize(crypt.sync->stepsize(1, st));

				if (vec.size() > 0) {
					std::cout << "CH synckey cl recv st:" << vec.size() << std::endl;

					vector<uint64_t> arrh, arrauth;
					arrh.resize(crypt.hash->HashSize());
					arrauth.resize(crypt.auth->MaxMSize(), 0);
					crypt.connect->recvV(vec.data(), vec.size() * 8);
					crypt.connect->recvV(arrauth.data(), arrauth.size() * 8);
					crypt.hash->Hash(vec.data(), vec.size(), arrh);
					int resauth = crypt.auth->Test(arrauth, arrh);

					if (resauth == 0)
						return 0;

					std::cout << "CH synckey cl recv end:" << bigint(vec, 1) << std::endl;
				}

				res = crypt.sync->step1(vec, vec1, st, crypt.rg);
				std::cout << "CH synckey cl step:" << res << std::endl;
				if (vec1.size() > 0 && res != -1) {
					crypt.connect->sendV(vec1.data(), vec1.size() * 8);

				}
				st++;
			} while (res == 0);
		}
		if (myType == 2) {
			std::cout << "CH synckey sv" << std::endl;
			do {
				vec.resize(crypt.sync->stepsize(2, st));

				if (vec.size() > 0) {
					std::cout << "CH synckey sv recv st:" << vec.size() << std::endl;
					crypt.connect->recvV(vec.data(), vec.size() * 8);
					std::cout << "CH synckey sv recv end:" << bigint(vec, 1) << std::endl;
				}

				res = crypt.sync->step2(vec, vec1, st, crypt.rg);
				std::cout << "CH synckey sv step:" << res << std::endl;
				if (vec1.size() > 0 && res != -1) {
					vector<uint64_t> arrh, arrauth;
					arrh.resize(crypt.hash->HashSize());
					crypt.hash->Hash(vec1.data(), vec1.size(), arrh);
					crypt.auth->Crypt(arrh, arrauth);
					std::cout << "CH synckey sv send H:" << bigint(arrh, 1) << std::endl;
					std::cout << "CH synckey sv send A:" << bigint(arrauth, 1) << std::endl;
					arrauth.resize(crypt.auth->MaxMSize(), 0);
					crypt.connect->sendV(vec1.data(), vec1.size() * 8);
					crypt.connect->sendV(arrauth.data(), arrauth.size() * 8);
				}
				st++;
			} while (res == 0);
		}

		if (res != 1)
			return 0;

		vector<uint64_t> arr2 = crypt.sync->getResult();
		arr2.resize((randBits + 63) / 64, 0);

		int addd = currKeyBits / 64;
		int addm = currKeyBits % 64;

		if (randBits % 64 != 0)
			arr2[(randBits - 1) / 64] &= ((uint64_t(1) << (randBits % 64)) - 1);

		std::cout << bigint(arr2, 1) << std::endl;

		for (int i = 0; i < arr2.size() && (i + addd)<keyres.size(); i++)
			keyres[i + addd] |= arr2[i] << addm;
		if (addm != 0)
			for (int i = 0; i < arr2.size() && (i + addd + 1)<keyres.size(); i++)
				keyres[i + addd + 1] |= arr2[i] >> (64 - addm);

		currKeyBits += randBits;
		if (currKeyBits >= keyNeedSize)
			keyReady = 1;
		std::cout << "---------------" << std::endl;
	} while (keyReady == 0);

	std::cout << bigint(keyres, 1) << std::endl;

	int testres = 1;

	if (myType == 1) {
		vector<uint8_t> testrandval, testrandvalret, mess;
		mess.resize(crypt.symm->GetDataScale(), 0);
		testrandval.resize(crypt.symm->GetDataScale(), 0);
		testrandvalret.resize(crypt.symm->GetDataScale(), 0);
		crypt.rg->GetRandom(testrandval.data(), testrandval.size());

		crypt.symm->SetKey(keyres);

		crypt.symm->Encode(testrandval.data(), mess.data(), mess.size());

		crypt.connect->sendV(mess.data(), mess.size());
		crypt.connect->recvV(mess.data(), mess.size());

		crypt.symm->Decode(mess.data(), testrandvalret.data(), mess.size());

		for (int i = 0; i < testrandval.size(); i++) {
			if ((testrandval[i] ^ testrandvalret[i]) != 0xFF)
				testres = 0;
		}

		for (int i = 0; i < testrandval.size() / 2; i++) {
			std::swap(testrandvalret[i], testrandvalret[testrandval.size() - 1 - i]);
		}
		for (int i = 0; i < testrandval.size(); i++)
			testrandvalret[i] = reverseBits(testrandvalret[i]);

		crypt.symm->Encode(testrandvalret.data(), mess.data(), mess.size());

		crypt.connect->sendV(mess.data(), mess.size());

		symmChainData = testrandvalret;
		symmChainDataDecode = testrandvalret;
	}
	if (myType == 2) {
		vector<uint8_t> testrandval, testrandvalret, mess;
		mess.resize(crypt.symm->GetDataScale(), 0);
		testrandval.resize(crypt.symm->GetDataScale(), 0);
		testrandvalret.resize(crypt.symm->GetDataScale(), 0);

		crypt.connect->recvV(mess.data(), mess.size());

		crypt.symm->SetKey(keyres);

		crypt.symm->Decode(mess.data(), testrandval.data(), mess.size());

		for (int i = 0; i < testrandval.size(); i++)
			testrandval[i] = ~testrandval[i];

		crypt.symm->Encode(testrandval.data(), mess.data(), mess.size());

		crypt.connect->sendV(mess.data(), mess.size());

		for (int i = 0; i < testrandval.size() / 2; i++) {
			std::swap(testrandval[i], testrandval[testrandval.size() - 1 - i]);
		}
		for (int i = 0; i < testrandval.size(); i++)
			testrandval[i] = reverseBits(testrandval[i]);

		crypt.connect->recvV(mess.data(), mess.size());

		crypt.symm->Decode(mess.data(), testrandvalret.data(), mess.size());

		for (int i = 0; i < testrandval.size(); i++) {
			if (testrandval[i] != testrandvalret[i])
				testres = 0;
		}
		symmChainData = testrandvalret;
		symmChainDataDecode = testrandvalret;

	}

	std::cout << "Test:" << testres << std::endl;


	return testres;

}

inline void NCryptChannel::SCrypt(void * data, int size) {
	int scale = crypt.symm->GetDataScale();
	if (size%scale != 0)
		throw;
	uint8_t *lbytedata = reinterpret_cast<uint8_t*>(data);

	for (int i = 0; i < size / scale; i++) {
		for (int r = 0; r < symmChainData.size(); r++)
			lbytedata[i * scale + r] ^= symmChainData[r];
		crypt.symm->Encode(lbytedata + i * scale, lbytedata + i * scale, scale);
		for (int r = 0; r < symmChainData.size(); r++)
			symmChainData[r] = lbytedata[i * scale + r];
	}

}

inline void NCryptChannel::SEncrypt(void * data, int size) {
	int scale = crypt.symm->GetDataScale();
	if (size%scale != 0)
		throw;
	uint8_t *lbytedata = reinterpret_cast<uint8_t*>(data);
	vector<uint8_t> temp(symmChainDataDecode.size());
	for (int i = 0; i < size / scale; i++) {
		for (int r = 0; r < symmChainDataDecode.size(); r++)
			temp[r] = lbytedata[i * scale + r];

		crypt.symm->Decode(lbytedata + i * scale, lbytedata + i * scale, scale);

		for (int r = 0; r < symmChainDataDecode.size(); r++)
			lbytedata[i * scale + r] ^= symmChainDataDecode[r];

		symmChainDataDecode = temp;
	}
}

inline bool NCryptChannel::senddata(const void * data, int size) {
	if (crypt.symm->GetDataScale() % 8 != 0)
		throw;
	if (size < 0)
		return 0;

	//hhhhhhhhssssssssSSSSSSSSdddddddddddddddddddRRRRRR

	int sizepadd = 3 * 8 + size;
	if (sizepadd % crypt.symm->GetDataScale() != 0)
		sizepadd += crypt.symm->GetDataScale() - (sizepadd % crypt.symm->GetDataScale());
	vector<uint64_t> vecsend(sizepadd / 8, 0);

	vecsend[1] = size;
	vecsend[2] = size;

	uint8_t *lbytevec = reinterpret_cast<uint8_t*>(&(vecsend[3]));
	const uint8_t *lbytedata = reinterpret_cast<const uint8_t*>(data);
	for (int i = 0; i < size; i++)
		lbytevec[i] = lbytedata[i];

	if (((3 * 8 + size) % crypt.symm->GetDataScale())  > 0)
		crypt.rg->GetRandom(&lbytevec[size], crypt.symm->GetDataScale() - ((3 * 8 + size) % crypt.symm->GetDataScale()));

	uint64_t xorhash = 0;
	for (int i = 2; i < vecsend.size(); i++)
		xorhash ^= vecsend[i];

	vecsend[0] = xorhash;

	SCrypt(vecsend.data(), vecsend.size() * 8);

	return crypt.connect->sendV(vecsend.data(), vecsend.size() * 8);

}

inline bool NCryptChannel::recvdata(vector<uint8_t>& vec) {
	if (crypt.symm->GetDataScale() % 8 != 0)
		throw;
	int sizehpadd = 3 * 8;
	if (sizehpadd % (crypt.symm->GetDataScale()) != 0)
		sizehpadd += crypt.symm->GetDataScale() - (sizehpadd % crypt.symm->GetDataScale());

	vector<uint64_t> vechead(sizehpadd / 8, 0);

	int resrecv = crypt.connect->recvV(vechead.data(), vechead.size() * 8);
	if (!resrecv)
		return 0;

	SEncrypt(vechead.data(), vechead.size() * 8);

	if (vechead[1] != vechead[2])
		return 0;
	int size = vechead[1];

	int sizepadd = 3 * 8 + size;
	if (sizepadd % crypt.symm->GetDataScale() != 0)
		sizepadd += crypt.symm->GetDataScale() - (sizepadd % crypt.symm->GetDataScale());

	if (sizepadd / 8 - vechead.size() > 0) {
		vechead.resize(sizepadd / 8, 0);

		int resrecv = crypt.connect->recvV(vechead.data() + sizehpadd / 8, vechead.size() * 8 - sizehpadd);
		if (!resrecv)
			return 0;

		SEncrypt(vechead.data() + sizehpadd / 8, vechead.size() * 8 - sizehpadd);
	}

	uint64_t xorhash = 0;
	for (int i = 2; i < vechead.size(); i++)
		xorhash ^= vechead[i];

	if (xorhash != vechead[0])
		return 0;

	uint8_t *lbytevec = reinterpret_cast<uint8_t*>(&(vechead[3]));
	vec.resize(size);
	for (int i = 0; i < size; i++) {
		vec[i] = lbytevec[i];
	}

	return 1;

}
