#pragma once
#include "NCryptBase.h"


class NCryptHashSHA3:public NCryptHash {
public:
	__declspec(dllexport) bool Hash(void*, uint64_t, std::vector<uint64_t>&);
	__declspec(dllexport) int HashSize();
	__declspec(dllexport) ~NCryptHashSHA3();
};

