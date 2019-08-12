#pragma once
#include "NCryptBase.h"
#include <numeric>

bigint gcdex(bigint a, bigint b, bigint & x, bigint & y) {
	if (a == 0) {
		x = 0; y = 1;
		return b;
	}
	bigint x1, y1;
	bigint d = gcdex(b%a, a, x1, y1);
	x = y1 - (b / a) * x1;
	y = x1;
	return d;
}

class NCryptAuthRSA :public NCryptAutentification {
private:
	bigint key_n, key_e, key_d;
public:
	//Максимальный размер подписываемого сообщения
	__declspec(dllexport) int MaxMSize();

	//Установить приватный ключ
	__declspec(dllexport) bool SetPrivate(std::vector<uint64_t> PrivateKey);

	//Установить публичный ключ
	__declspec(dllexport) bool SetToken(std::vector<uint64_t> PublicKey);

	//Сравнение зашифрованного хеша с хешом сообщения
	__declspec(dllexport) bool Test(std::vector<uint64_t> arrm, std::vector<uint64_t> arrh);

	//Генерирование ключей
	__declspec(dllexport) bool Generate(int64_t size, std::vector<uint64_t>& PrivateKey, std::vector<uint64_t>& PublicKey, std::shared_ptr<NRandomGen> rg);

	//Шифрование хеша 
	__declspec(dllexport) bool Crypt(std::vector<uint64_t> harr, std::vector<uint64_t> &res);

	__declspec(dllexport) ~NCryptAuthRSA();
};
