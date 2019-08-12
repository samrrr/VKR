// ����������� ����� �����

#pragma once
#include "NCryptBase.h"

// ��������� � �������� �� ������ ���������� ����������
class NCryptSTR {
public:
	std::shared_ptr<NCryptAutentification> auth;
	std::shared_ptr<NCryptHash> hash;
	std::shared_ptr<NCryptSync> sync;
	std::shared_ptr<NCryptSymmetric> symm;
	std::shared_ptr<NCryptConnection> connect;
	std::shared_ptr<NRandomGen> rg;

};

// �������� ������� ����� � �����
uint8_t reverseBits(uint8_t a) {
	// 76543210
	// 6 4 2 0 
	//  7 5 3 1
	// 67452301
	//   67  23
	// 45  01
	// 45670123
	// 0123
	//     4561
	// 01234567
	a = (a & 0x55) << 1 | (a & 0xAA) >> 1;
	a = (a & 0x33) << 2 | (a & 0xCC) >> 2;
	a = (a & 0x0F) << 4 | (a & 0xF0) >> 4;
	return a;
}

// ����� ������������ ������ �����
class NCryptChannel {
private:
	NCryptSTR crypt;
	vector<uint8_t> symmChainData;
	vector<uint8_t> symmChainDataDecode;
	int myType = 0;

	// ����������
	void SCrypt(void *data, int size);

	// ����������
	void SEncrypt(void *data, int size);

public:

	// �����������(������������� ������)
	__declspec(dllexport) NCryptChannel(NCryptSTR &s);

	// ���������� � ��������
	__declspec(dllexport) bool Connect(std::string ip, int port);

	// �������� ��������
	__declspec(dllexport) bool Listen(int port);

	// ������������� ������ �������
	__declspec(dllexport) bool synckey();

	// ������� ������ ������
	__declspec(dllexport) bool senddata(const void *data, int size);

	// ���� ������ ������
	__declspec(dllexport) bool recvdata(vector<uint8_t> &vec);
};


