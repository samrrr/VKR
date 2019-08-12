#pragma once
#include "NCryptBase.h"

class NCryptHashXor :public NCryptHash {
private:
	uint64_t size = 128;
public:
	__declspec(dllexport) bool Hash(void*, uint64_t, std::vector<uint64_t>&);
	__declspec(dllexport) bool setSize(uint64_t size);
	__declspec(dllexport) ~NCryptHashXor();
};
