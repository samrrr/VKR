#pragma once

#include "NCryptBase.h"

class NCryptSymIDEA :public NCryptSymmetric {
private:
	uint16_t K[6 * 8 + 4];
	uint16_t K2[6 * 8 + 4];
	bool haveK2;
public:
	__declspec(dllexport) const uint64_t GetDataScale();
	__declspec(dllexport) const uint64_t GetKeySize();
	__declspec(dllexport) bool SetKey(std::vector<uint64_t>);
	__declspec(dllexport) bool Encode(void*, void*, uint64_t);
	__declspec(dllexport) bool Decode(void*, void*, uint64_t);
	__declspec(dllexport) ~NCryptSymIDEA();
	void genK2();
};

