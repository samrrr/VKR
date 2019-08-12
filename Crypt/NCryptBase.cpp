#include "NCryptBase.h"

NCryptSymmetric::~NCryptSymmetric() {

}

NCryptAsymmetric::~NCryptAsymmetric() {

}

NCryptAutentification::~NCryptAutentification() {

}

NCryptHash::~NCryptHash(){

}

#ifdef _WIN64
#include <winsock2.h>
#include <windows.h>
#undef max
#undef min
#include <bcrypt.h>
#pragma comment( lib, "Bcrypt.lib" )

#include <winternl.h>

bool NRandomGenWin::GetRandom(void *data, int64_t size)
{
	NTSTATUS    Status;

	Status = BCryptGenRandom(
		NULL,                      
		(PUCHAR)data,                   
		static_cast<ULONG>(size),           
		BCRYPT_USE_SYSTEM_PREFERRED_RNG);

	if (!NT_SUCCESS(Status))
	{
		return false;
	}
	return true;
}
#endif

bigint randomNumberGen(std::shared_ptr<NRandomGen> rg,int size, int addbit) {
	if (size < 1)
		throw;
	if (size == 1) {
		if (addbit)
			return 1;
		char ch;
		if (!rg->GetRandom(&ch, 1))
			throw;
		return ch % 2;
	}

	vector<uint64_t> arr;
	arr.resize((size - 1) / 64 + 1, 0);

	if (!rg->GetRandom(arr.data(), arr.size() * sizeof(uint64_t)))
		throw;

	if (addbit)
		arr[(size - 1) / 64] |= (uint64_t)1 << ((size - 1) % 64);
	
	if (size % 64 != 0)
		arr[(size - 1) / 64] &= (((uint64_t)1 << (size - (size - 1) / 64 * 64)) - 1);

	return bigint(arr,1);
}

bigint randomPrime(int size, std::shared_ptr<NRandomGen> rg) {

	bigint res = nextPrime(randomNumberGen(rg,size));

	if (
		res.data().size() > (size - 1) / 64 + 1 ||
		((size % 64 != 0) && res.data()[(size - 1) / 64]>((uint64_t)1 << ((size) % 64)))
		) {
		res = nextPrime(res >> 1);
	}
	return res;
}

bigint randomExtraPrime(int size, std::shared_ptr<NRandomGen> rg)
{
	bigint res = nextPrimeEX(randomNumberGen(rg, size));

	if (
		res.data().size() > (size - 1) / 64 + 1 ||
		((size % 64 != 0) && res.data()[(size - 1) / 64] > ((uint64_t)1 << ((size) % 64)))
		) {
		res = nextPrimeEX(res >> 1);
	}
	 

	return res;
}

NCryptConnection::~NCryptConnection()
{
}

NCryptSync::~NCryptSync()
{
}
