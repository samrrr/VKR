#pragma once
#include "NCryptBase.h"
#include <sstream>

class NCryptSyncDiffHelm : public NCryptSync {
private:
	bigint p,g,A,B,K,a,b;
	int size = 1026,havep=0;
	int numsz = (1026 + 63) / 64;
public:
	//��������� p
	__declspec(dllexport) void setp(bigint _p);

	//��������� ������� p
	__declspec(dllexport) void setsz(int sz);

	//���������� ���� ��� ������� ���������
	__declspec(dllexport) int step1(const vector<uint64_t> &arrin, vector<uint64_t> &arrout, int step, std::shared_ptr<NRandomGen>rg);
	
	//���������� ���� ��� ������� ���������
	__declspec(dllexport) int step2(const vector<uint64_t> &arrin, vector<uint64_t> &arrout, int step, std::shared_ptr<NRandomGen>rg);
	
	//������ ������� ������ ��� ����
	__declspec(dllexport) int stepsize(int type, int step);

	//��������� ������ �������
	__declspec(dllexport) vector<uint64_t> getResult();

	//���������� ��������� ��� � �������
	__declspec(dllexport) int randomBitsCount();
};

