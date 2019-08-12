#pragma once

#include "NCryptBase.h"

class NCryptXor:public NCryptSymmetric {
public:
	__declspec(dllexport) const uint64_t GetDataScale() {
		return 1;
	}
	__declspec(dllexport) const bool TryKeySize(uint64_t size) {
		return size>0;
	}
	__declspec(dllexport) bool SetKey(std::vector<uint8_t> newkey) {
		if(newkey.size()>0)
			key = newkey;
	}
	__declspec(dllexport) bool Encode(void* src, uint64_t srcsz, void* dest, uint64_t destsz) {
		if (srcsz > destsz)
			return false;
		uint8_t *isrc = reinterpret_cast<uint8_t*>(src);
		uint8_t *idest = reinterpret_cast<uint8_t*>(dest);
		for (int i = 0; i < srcsz; i++) {
			idest[i]=isrc[i]^key[i%key.size()];
		}
		return true;
	}
	__declspec(dllexport) bool Encode(void*src, uint64_t srcsz) {

		uint8_t *isrc = reinterpret_cast<uint8_t*>(src);

		for (int i = 0; i < srcsz; i++) {
			isrc[i] ^= key[i%key.size()];
		}
		return true;
	}
private:
	std::vector<uint8_t> key;
};