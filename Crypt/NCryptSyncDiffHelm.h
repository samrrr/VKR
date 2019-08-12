#pragma once
#include "NCryptBase.h"
#include <sstream>

class NCryptSyncDiffHelm : public NCryptSync {
private:
	bigint p,g,A,B,K,a,b;
	int size = 1026,havep=0;
	int numsz = (1026 + 63) / 64;
public:
	//Установка p
	__declspec(dllexport) void setp(bigint _p);

	//Установка размера p
	__declspec(dllexport) void setsz(int sz);

	//Выполнение шага для первого участника
	__declspec(dllexport) int step1(const vector<uint64_t> &arrin, vector<uint64_t> &arrout, int step, std::shared_ptr<NRandomGen>rg);
	
	//Выполнение шага для второго участника
	__declspec(dllexport) int step2(const vector<uint64_t> &arrin, vector<uint64_t> &arrout, int step, std::shared_ptr<NRandomGen>rg);
	
	//Размер входных данных для шага
	__declspec(dllexport) int stepsize(int type, int step);

	//Получение общего секрета
	__declspec(dllexport) vector<uint64_t> getResult();

	//Количество случайных бит в секрете
	__declspec(dllexport) int randomBitsCount();
};

