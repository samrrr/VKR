#include "NCryptHashXor.h"

bool NCryptHashXor::Hash(void *src, uint64_t srcsz, std::vector<uint64_t>&res)
{
	if (srcsz % 8 != 0)
		throw;
	uint64_t *isrc = reinterpret_cast<uint64_t *>(src);

	res= std::vector<uint64_t>(size,0);

	for (int i = 0; i < srcsz; i++) {
		res[i%size]^=isrc[i];
	}

	return true;
}

bool NCryptHashXor::setSize(uint64_t size) {
	if (size == 0)
		return false;
	this->size = size;
}

NCryptHashXor::~NCryptHashXor() {
}
