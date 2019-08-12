#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <algorithm>

using uint = unsigned int;
using std::vector;
using std::string;
using std::min;
using std::max;

class bigint
{
	vector<uint64_t> vdata;
	int sign;
	void opadd(const bigint &);
	void opsub(const bigint &);
	void oprr();
	int opbigger(const bigint &)const;
	int firstbitpos()const;
	int lastbitpos()const;
	void fit();
public:
	__declspec(dllexport) bigint();
	__declspec(dllexport) bigint(int64_t);
	__declspec(dllexport) bigint(int a);
	__declspec(dllexport) bigint(uint64_t);
	__declspec(dllexport) bigint(const bigint &);
	__declspec(dllexport) bigint(bigint &&);
	__declspec(dllexport) bigint(vector<uint64_t>, int);
	__declspec(dllexport) bigint(string);
	__declspec(dllexport) uint64_t getuint64_t() const;
	__declspec(dllexport) int getsign();
	__declspec(dllexport) void maskand(int size);
	__declspec(dllexport) bigint& operator= (const bigint &);
	__declspec(dllexport) bigint friend operator+ (const bigint &, const bigint &);
	__declspec(dllexport) bigint friend operator- (const bigint &, const bigint &);
	__declspec(dllexport) bigint friend operator* (const bigint &, const bigint &);
	__declspec(dllexport) bigint friend operator/ (const bigint &, const bigint &);
	__declspec(dllexport) bigint friend operator% (const bigint &, const bigint &);
	__declspec(dllexport) bool friend operator> (const bigint &, const bigint &);
	__declspec(dllexport) bool friend operator< (const bigint &, const bigint &);
	__declspec(dllexport) bool friend operator== (const bigint &, const bigint &);
	__declspec(dllexport) bool friend operator!= (const bigint &, const bigint &);
	__declspec(dllexport) bigint friend operator<< (const bigint &, int);
	__declspec(dllexport) bigint friend operator>> (const bigint &, int);
	__declspec(dllexport) friend std::ostream& operator<< (std::ostream &, const bigint &);
	__declspec(dllexport) friend bigint powmod(const bigint &, const bigint &, const bigint &);
	__declspec(dllexport) friend bool testp(const bigint &a, int rounds);
	__declspec(dllexport) friend bigint sqrt(const bigint &a);
	__declspec(dllexport) string todec();
	__declspec(dllexport) vector<uint64_t> &data();
	__declspec(dllexport) ~bigint();
	static void test();
};

bigint nextPrime(bigint num);
bigint nextPrimeEX(bigint num);


