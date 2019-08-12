// ������� ����������� ������.

#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "bigint.h"

// ����������� ������� ����� ���������� ��������� �����
class NRandomGen {
public:
	// ��������� ���������� �����
	virtual bool GetRandom(void *, int64_t) = 0;
};

// ����������� ����� ������������� ����������
class NCryptSymmetric {
public:
	// �������� ��� ������ � ������
	virtual const uint64_t GetDataScale() = 0;

	// �������� ������ ����� ������ � ������
	virtual const uint64_t GetKeySize() = 0;

	// ���������� ����
	virtual bool SetKey(std::vector<uint64_t>) = 0;

	// �����������
	virtual bool Encode(void*, void*, uint64_t) = 0;

	// ������������
	virtual bool Decode(void*, void*, uint64_t) = 0;

	virtual __declspec(dllexport)  ~NCryptSymmetric();
};

// ����������� ����� ��������������� ����������
class NCryptAsymmetric {
public:
	// �������� ��� ������ � ������
	virtual const uint64_t GetDataScale() = 0;

	// ���������� ��������� ����
	virtual bool SetPrivateKey(std::vector<uint64_t>) = 0;

	// ���������� ��������� ����
	virtual bool SetPublicKey(std::vector<uint64_t>) = 0;

	// �����������
	virtual bool Crypt(void*, uint64_t, void*, uint64_t) = 0;

	// ������������
	virtual bool Encrypt(void*, uint64_t) = 0;

	virtual __declspec(dllexport)  ~NCryptAsymmetric();
};

// ����������� ����� ��������������(�������)
class NCryptAutentification {
public:
	// ������������ ������ �������������� ���������
	virtual int MaxMSize() = 0;

	// ���������� ��������� ����
	virtual bool SetPrivate(std::vector<uint64_t>) = 0;

	// ���������� ��������� ����
	virtual bool SetToken(std::vector<uint64_t>) = 0;

	// ��������� �������������� ���� � ����� ���������
	virtual bool Test(std::vector<uint64_t> ,std::vector<uint64_t>) = 0;

	// ������������� ������
	virtual bool Generate(int64_t, std::vector<uint64_t>&, std::vector<uint64_t>&, std::shared_ptr<NRandomGen>) = 0;

	// ���������� ���� 
	virtual bool Crypt(std::vector<uint64_t>,std::vector<uint64_t> &) = 0;

	virtual __declspec(dllexport)  ~NCryptAutentification();
};

// ����������� ����� ��� �������
class NCryptHash {
public:
	// ��� �� ������
	virtual bool Hash(void*, uint64_t, std::vector<uint64_t>&) = 0;

	// ��������� ������� ����
	virtual int HashSize() = 0;

	virtual __declspec(dllexport) ~NCryptHash();
};

// ����������� ����� ������������� ������ �������
class NCryptSync {
public:
	// ���������� ���� ��� ������� ���������
	virtual int step1(const vector<uint64_t> &, vector<uint64_t> &, int, std::shared_ptr<NRandomGen>rg) = 0;

	// ���������� ���� ��� ������� ���������
	virtual int step2(const vector<uint64_t> &, vector<uint64_t> &, int, std::shared_ptr<NRandomGen>rg) = 0;

	// ������ ������� ������ ��� ����
	virtual int stepsize(int type, int step) = 0;

	// ��������� ������ �������
	virtual vector<uint64_t> getResult() = 0;

	// ���������� ��������� ��� � �������
	virtual int randomBitsCount() = 0;

	virtual __declspec(dllexport) ~NCryptSync();
};

// ����������� ����� ������������� ���������� 
class NCryptConnection {
public:
	// ���������� � ��������
	virtual bool connectToServer(const char* ip, uint16_t port) = 0;

	// �������� ��������
	virtual bool listenClients(uint16_t port) = 0;

	// ������� ������
	virtual bool sendV(void*, uint64_t) = 0;

	// ���� ������
	virtual bool recvV(void*, uint64_t) = 0;

	// ����� ����������
	virtual void disconnectConn() = 0;

	// ��������� ���������� �����             
	virtual __declspec(dllexport) ~NCryptConnection();
};


#ifdef _WIN64
// ��������� ��������� ������
class NRandomGenWin :public NRandomGen {
	__declspec(dllexport) bool GetRandom(void *, int64_t);
};
#define NBASICRG std::shared_ptr<NRandomGen>(static_cast<NRandomGen*>(new NRandomGenWin()))
#endif


// ��������� ���������� ����� ��������� �������
extern __declspec(dllexport) bigint randomNumberGen(std::shared_ptr<NRandomGen> rg, int size, int addbit = 1);

// ��������� ���������� �������� ����� ��������� �������
extern __declspec(dllexport) bigint randomPrime(int size, std::shared_ptr<NRandomGen>rg);

// ��������� ����� p ��������� ������� ������, ��� p � (p-1)/2 �������
extern __declspec(dllexport) bigint randomExtraPrime(int size, std::shared_ptr<NRandomGen>rg);
