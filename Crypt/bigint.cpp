#include "bigint.h"


inline uint64_t addcarry(uint64_t a, uint64_t b, bool &c) {
	uint64_t res;
	res = a + b + c;
	c = c ? res <= b : res < b;
	return res;
}

inline uint64_t subcarry(uint64_t a, uint64_t b, bool &c) {
	uint64_t res;
	res = a - b - c;
	c = c ? res >= a : res > a;
	return res;
}
inline uint64_t rrcarry(uint64_t a, bool &c) {
	uint64_t res;
	res = (a>>1);
	if (c)
		res |= 0x8000000000000000;
	c = a % 2 == 1;;
	return res;
}
inline void rlbig(uint64_t a, int sdv, uint64_t &res1, uint64_t &res2) {
	res1 = a << (sdv);
	res2 = a >> (64 - sdv);
}
inline void rrbig(uint64_t a, int sdv, uint64_t &res1, uint64_t &res2) {
	res1 = a << (64 - sdv);
	res2 = a >> (sdv);
}





bigint::bigint()
{
	sign = 1;
}

bigint::bigint(int64_t a)
{
	if (a < 0) {
		sign = -1;
		vdata.resize(1);
		vdata[0] = -a;
	}
	else
		if (a > 0) {
			sign = 1;
			vdata.resize(1);
			vdata[0] = a;
		}
		else {
			sign = 1;
			vdata.resize(0);
		}
}
bigint::bigint(int a)
{
	if (a < 0) {
		sign = -1;
		vdata.resize(1);
		vdata[0] = -a;
	}
	else
		if (a > 0) {
			sign = 1;
			vdata.resize(1);
			vdata[0] = a;
		}
		else {
			sign = 1;
			vdata.resize(0);
		}
}
bigint::bigint(uint64_t a)
{
	sign = 1;
	vdata.resize(1);
	vdata[0] = a;
	fit();
}

bigint::bigint(const bigint & a)
{
	sign = a.sign;
	vdata = a.vdata;
}

bigint::bigint(bigint &&a)
{
	sign = a.sign;
	std::swap(vdata, a.vdata);
}

bigint::bigint(vector<uint64_t> data, int sign=1):vdata(data),sign(sign)
{
}

bigint::bigint(string s)
{
	sign = 1;
	if (s.size() == 0)
		throw;
	if (s.size() == 1) {
		if (s[0] >= '0' && s[0] <= '9') {
			vdata.resize(1);
			vdata[0] = s[0] - '0';
		}
		else
			throw;
		return;
	}
	int dd = 0;
	if (s[0] == '-') {
		sign = -1;
		dd = 1;
	}
	if (s.size() == 2 && dd == 1) {
		if (s[1] >= '0' && s[1] <= '9') {
			vdata.resize(1);
			vdata[0] = s[1] - '0';
		}
		else
			throw;
		return;
	}
	if (s[dd] == '0' && (s[dd + 1] == 'x' || s[dd + 1] == 'X')) {

		for (int i = dd+2; i < s.size(); i++) {
			if ((s[i] < '0' || s[i] > '9') && (s[i] < 'a' || s[i] > 'f')) {
				vdata.resize(0);
				sign = 1;
				throw;
			}
			if (s[i] >= '0' && s[i] <= '9') {
				*this = *this * 16 + (s[i] - '0');
			}
			if (s[i] >= 'a' && s[i] <= 'f') {
				*this = *this * 16 + (s[i] - 'a'+10);
			}
		}
		return;
	}
	for (int i = dd; i <s.size(); i++) {
		if (s[i] < '0' || s[i] > '9') {
			vdata.resize(0);
			sign = 1;
			throw;
		}
		*this = *this * 10 + (s[i] - '0');
	}
}

uint64_t bigint::getuint64_t() const
{
	if (vdata.size() > 0)
		return vdata[0];
	return 0;
}
int bigint::getsign()
{
	return sign;
}

void bigint::maskand(int size)
{
	
	int div = size / 64;
	if (div >= vdata.size())
		return;
	int mod = size % 64;
	
	vdata[div] = vdata[div] & ((uint64_t(1) << mod) - 1);
	vdata.resize(div + 1);
}

bigint & bigint::operator=(const bigint &a)
{
	if (this == &a)
		return *this;

	sign = a.sign;
	vdata = a.vdata;

	// возвращаем текущий объект
	return *this;
}

bigint operator+(const bigint &a, const bigint &b)
{
	bigint res;
	if (a.sign == b.sign) {
		res.vdata.resize(std::max(a.vdata.size(), b.vdata.size()) + 1, 0);
		for (int i = 0; i < a.vdata.size(); i++) {
			res.vdata[i] = a.vdata[i];
		}
		res.sign = a.sign;
		res.opadd(b);
	}
	else {
		switch (a.opbigger(b)) {
		case 1:
			res.vdata.resize(std::max(a.vdata.size(), b.vdata.size()), 0);
			res.sign = a.sign;
			for (int i = 0; i < a.vdata.size(); i++) {
				res.vdata[i] = a.vdata[i];
			}
			res.opsub(b);
			break;
		case -1:
			res.vdata.resize(std::max(a.vdata.size(), b.vdata.size()), 0);
			res.sign = b.sign;
			for (int i = 0; i < b.vdata.size(); i++) {
				res.vdata[i] = b.vdata[i];
			}
			res.opsub(a);
			break;
		case 0://zero
			break;

		}
	}

	res.fit();
	return res;
}

bigint operator-(const bigint &a, const bigint &b)
{
	bigint res;
	if (a.sign != b.sign) {
		res.vdata.resize(std::max(a.vdata.size(), b.vdata.size() ) + 1, 0);
		for (int i = 0; i < a.vdata.size(); i++) {
			res.vdata[i] = a.vdata[i];
		}
		res.sign = a.sign;
		res.opadd(b);
	}
	else {
		switch (a.opbigger(b)) {
		case 1:
			res.vdata.resize(std::max(a.vdata.size(), b.vdata.size()), 0);
			res.sign = a.sign;
			for (int i = 0; i < a.vdata.size(); i++) {
				res.vdata[i] = a.vdata[i];
			}
			res.opsub(b);
			break;
		case -1:
			res.vdata.resize(std::max(a.vdata.size(), b.vdata.size()), 0);
			res.sign = -b.sign;
			for (int i = 0; i < b.vdata.size(); i++) {
				res.vdata[i] = b.vdata[i];
			}
			res.opsub(a);
			break;
		case 0://zero
			break;
		}
	}
	res.fit();
	return res;
}

inline void mulbig(uint64_t a, uint64_t b, uint64_t &res1, uint64_t &res2) {
	uint64_t a0, a1, b0, b1;
	uint64_t r00, r01, r10, r11;
	uint64_t mid;
	a0 = a & 0x00000000FFFFFFFF;
	a1 = a >> 32;
	b0 = b & 0x00000000FFFFFFFF;
	b1 = b >> 32;
	
	r00 = a0 * b0;
	r01 = a0 * b1;
	r10 = a1 * b0;
	r11 = a1 * b1;

	res1 = r00;
	res2 = r11;

	bool c = 0;
	mid=addcarry(r01, r10, c);
	if(c)
		res2 += (uint64_t)1 << 32;
	c = 0;
	res1 = addcarry(res1,mid<<32,c );
	if (c)
		res2 += 1;
	res2 += mid >> 32;

}

bigint operator*(const bigint &a, const bigint &b)
{
	bigint res;
	auto N = a.vdata.size() + b.vdata.size();
	res.vdata.resize(N);
	res.sign = a.sign*b.sign;
	for (int i = 0; i < a.vdata.size(); i++) {
		bigint p;
		p.vdata.resize(N);

		for (int r = 0; r < b.vdata.size(); r += 2) {
			mulbig(a.vdata[i], b.vdata[r], p.vdata[i + r], p.vdata[i + r + 1]);
		}
		res.opadd(p);
		p.vdata.resize(0);
		p.vdata.resize(N);
		for (int r = 1; r < b.vdata.size(); r += 2) {
			mulbig(a.vdata[i], b.vdata[r], p.vdata[i + r], p.vdata[i + r + 1]);
		}
		res.opadd(p);
	}
	res.fit();
	return res;
}

int getbitnum(uint64_t a) {
	int res = 0;
	if (a & 0xFFFFFFFF00000000) { res += 32; a >>= 32; }
	if (a & 0x00000000FFFF0000) { res += 16; a >>= 16; }
	if (a & 0x000000000000FF00) { res += 8; a >>= 8; }
	if (a & 0x00000000000000F0) { res += 4; a >>= 4; }
	if (a & 0x000000000000000C) { res += 2; a >>= 2; }
	if (a & 0x0000000000000002) { res += 1; a >>= 1; }
	return res;
}

int getlbitnum(uint64_t a) {
	int res = 0;
	if (!(a & 0x00000000FFFFFFFF)) { res += 32; a >>= 32; }
	if (!(a & 0x000000000000FFFF)) { res += 16; a >>= 16; }
	if (!(a & 0x00000000000000FF)) { res += 8; a >>= 8; }
	if (!(a & 0x000000000000000F)) { res += 4; a >>= 4; }
	if (!(a & 0x0000000000000003)) { res += 2; a >>= 2; }
	if (!(a & 0x0000000000000001)) { res += 1; a >>= 1; }
	return res;
}

bigint operator/(const bigint &a, const bigint &b)
{
	bigint resdiv;
	int numdiff = a.firstbitpos() - b.firstbitpos();
	//std::cout << "a:" << a << std::endl;
	//std::cout << "b:" << b << std::endl;
	//std::cout << "af:" << a.firstbitpos() << std::endl;
	//std::cout << "bf:" << b.firstbitpos() << std::endl;
	if (numdiff<0)
		return resdiv;
	bigint bmovv = b << numdiff;
	bigint resmod = a;
	//std::cout << "bmovv st:" << bmovv << std::endl;
	resdiv.vdata.resize(a.vdata.size(), 0);
	for (int i = numdiff; i >= 0; i--) {
		//std::cout << "resmod:" << resmod << std::endl;
		//std::cout << "bmovv :" << bmovv << std::endl;
		//std::cout << "resdiv:" << resdiv << std::endl;
		if (resmod.opbigger(bmovv) != -1) {
			resmod.opsub(bmovv);
			resdiv.vdata[i / 64] |= (uint64_t)1 << (i % 64);
		}
		bmovv.oprr();

	}

	resdiv.fit();
	resmod.fit();
	resdiv.sign = a.sign*b.sign;
	return resdiv;
}

bigint operator%(const bigint &a, const bigint &b)
{
	bigint resdiv;
	bigint resmod = a;
	int numdiff = a.firstbitpos() - b.firstbitpos();
	//std::cout << "a:" << a << std::endl;
	//std::cout << "b:" << b << std::endl;
	//std::cout << "af:" << a.firstbitpos() << std::endl;
	//std::cout << "bf:" << b.firstbitpos() << std::endl;
	if (numdiff<0)
		return resmod;
	bigint bmovv = b << numdiff;
	//std::cout << "bmovv st:" << bmovv << std::endl;
	resdiv.vdata.resize(a.vdata.size(), 0);
	for (int i = numdiff; i >= 0; i--) {
		//std::cout << "resmod:" << resmod << std::endl;
		//std::cout << "bmovv :" << bmovv << std::endl;
		//std::cout << "resdiv:" << resdiv << std::endl;
		if (resmod.opbigger(bmovv) != -1) {
			resmod.opsub(bmovv);
			resdiv.vdata[i / 64] |= (uint64_t)1 << (i % 64);
		}
		bmovv.oprr();

	}

	resdiv.fit();
	resmod.fit();
	resmod.sign = a.sign*b.sign;
	return resmod;
}

bool operator>(const bigint &a, const bigint &b)
{
	if (a.sign == -1 && b.sign == -1)
		return b.opbigger(a)==1;
	if (a.sign == 1 && b.sign == 1)
		return a.opbigger(b)==1;
	if (a.sign == -1 && b.sign == 1)
		return 0;
	if (a.sign == 1 && b.sign == -1)
		return (a != 0) || (b != 0);
}
bool operator<(const bigint &a, const bigint &b)
{
	return b>a;
}

bool operator==(const bigint &a, const bigint &b)
{
	if (a.sign == b.sign) {
		for (auto i = 0; i < min(a.vdata.size(), b.vdata.size()); i++) {
			if (a.vdata[i] != b.vdata[i])
				return false;
		}
		if (a.vdata.size() > b.vdata.size()) {
			for (auto i = min(a.vdata.size(), b.vdata.size()); i < max(a.vdata.size(), b.vdata.size()); i++) {
				if (a.vdata[i] != 0)
					return false;
			}
		}
		if (b.vdata.size() > a.vdata.size()) {
			for (auto i = min(a.vdata.size(), b.vdata.size()); i < max(a.vdata.size(), b.vdata.size()); i++) {
				if (b.vdata[i] != 0)
					return false;
			}
		}

		return true;
	}
	else {
		for (const auto &e : a.vdata)
			if (e != 0)
				return false;
		for (const auto &e : b.vdata)
			if (e != 0)
				return false;
		return true;
	}
}

bool operator!=(const bigint &a, const bigint &b)
{
	return !(a==b);
}

bigint operator<<(const bigint &a, int b)
{
	bigint res;

	int sddiv = b / 64;
	int sdmod = b % 64;

	res.vdata.resize(a.vdata.size() + 1 + sddiv, 0);
	res.sign=a.sign;

	if(sdmod!=0)
		for (int i = 0; i < a.vdata.size(); i++) {
			uint64_t p1, p2;
			rlbig(a.vdata[i], sdmod, p1, p2);
			res.vdata[i + sddiv] |= p1;
			res.vdata[i + sddiv + 1] |= p2;
		}
	else
		for (int i = 0; i < a.vdata.size(); i++) {
			res.vdata[i + sddiv] |= a.vdata[i];
		}

	res.fit();
	return res;
}

bigint operator>>(const bigint &a, int b)
{
	bigint res;

	int sddiv = b / 64;
	int sdmod = b % 64;

	res.vdata.resize(a.vdata.size(), 0);
	res.sign = a.sign;

	if (sdmod != 0)
		for (size_t i = a.vdata.size() ; i-- > 0;) {
			uint64_t p1, p2;
			rrbig(a.vdata[i], sdmod, p1, p2);
			if (i - sddiv == 0) {
				res.vdata[i - sddiv] |= p2;
				break;
			}
			res.vdata[i - sddiv - 1] |= p1;
			res.vdata[i - sddiv] |= p2;
		}
	else
		for (size_t i = a.vdata.size() ; i-- > 0;) {
			res.vdata[i - sddiv] |= a.vdata[i];
		}
		

	res.fit();
	return res;
}

std::ostream & operator<<(std::ostream &out, const bigint &a)
{
	if (a.sign == -1)
		out << '-';
	for (size_t i = a.vdata.size() ;  i-- > 0;) {
		out << std::setfill('0') << std::setw(16) << std::hex<< a.vdata[i];
	}
	out<< std::dec;
	return out;
}

#include <chrono>
using namespace std;
bigint powmod(const bigint &a, const bigint &b, const bigint &c)
{

	bigint res(1);

	for (int i = b.firstbitpos(); i >= 0; i--) {
		res = (res * res);
		res = res % c;
		if (b.vdata[i / 64] & ((uint64_t)1 << (i % 64))) {
			res = (res * a) % c;
		}
	}

	return res;
}


bool testp(const bigint &n,int rounds = 100)
{

	int res = 1;
	bigint b = n - 1;
	int s=b.lastbitpos();
	bigint t = b >> s;
	for (int i = 0; i < rounds && res; i++) {
		bigint a,x;
		a.vdata.resize(n.vdata.size()+1);
		for (auto &e : a.vdata)
			e = rand() + ((uint64_t)rand() << 16) + ((uint64_t)rand() << 32) + ((uint64_t)rand() << 48);
		a = (a % (n - 3)) + 2;
		std::cout << "-";

		x = powmod(a, t, n);



		if (x != 1 && x != n - 1) {
			int f = 0;
			for (int r = 0; r < s - 1 && f==0; r++) {
				x = x * x % n;
				if (x == 1)
					res = 0;
				if (x == n - 1)
					f = 1;
			}
			if (f == 0)
				res = 0;
			
		}
	}
	std::cout << std::endl;

	return res;
}

bigint sqrt(const bigint & a)
{
	bigint rslt = a;
	bigint div = a;
	while (1)
	{
		div = (a / div + div) / 2;
		if (rslt.opbigger(div))
			rslt = div;
		else
			return rslt;
	}
}

bigint nextPrime(bigint num) {

	bigint p1 = num + 1;

	const static int osn[100] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71
		,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173
		,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281
		,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409
		,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541 };

	int val[100];
	int L = 100;
	for (int i = 0; i < L; i++) {
		val[i] = static_cast<int>((p1%osn[i]).getuint64_t());
		if (num < osn[i])
			return osn[i];
	}


	while (1) {
		int inc = 0;

		int t1 = 0;
		while (!t1) {
			t1 = 1;
			inc++;
			for (int i = 0; i < L; i++) {
				val[i] = (val[i] + 1) % osn[i];
				if (val[i] == 0)
					t1 = 0;
			}
		}
		p1 = p1 + inc;
		if (testp(p1))
			return p1;
	}
}

bigint nextPrimeEX(bigint num) {

	if (num < 2048)
		throw;

	bigint p1 = num + 1,p2;
	if (p1 % 2 == 0)
		p1 = p1 + 1;
	p2 = (p1 - 1) / 2;



	const static int osn[100] = { 2,3,5,7,11,13,17,19,23,29,31,37,41,43,47,53,59,61,67,71
		,73,79,83,89,97,101,103,107,109,113,127,131,137,139,149,151,157,163,167,173
		,179,181,191,193,197,199,211,223,227,229,233,239,241,251,257,263,269,271,277,281
		,283,293,307,311,313,317,331,337,347,349,353,359,367,373,379,383,389,397,401,409
		,419,421,431,433,439,443,449,457,461,463,467,479,487,491,499,503,509,521,523,541 };

	int val1[100], val2[100];
	int L = 100;
	for (int i = 0; i < L; i++) {
		val1[i] = static_cast<int>((p1%osn[i]).getuint64_t());
		val2[i] = static_cast<int>((p2%osn[i]).getuint64_t());
	}


	while (1) {
		int inc = 0;

		int t1 = 0;
		while (!t1) {
			t1 = 2;
			inc++;
			for (int i = 0; i < L; i++) {
				val1[i] = (val1[i] + 2) % osn[i];
				if (val1[i] == 0)
					t1 = 0;
			}
			for (int i = 0; i < L; i++) {
				val2[i] = (val2[i] + 1) % osn[i];
				if (val2[i] == 0)
					t1 = 0;
			}
		}
		p1 = p1 + inc + inc;
		p2 = p2 + inc;

		int tt = 1;

		for (int i = 0; i < 100 && tt; i++) {
			if (!testp(p1, 1))
				tt = 0;
			cout << tt;
			if(tt)
				if (!testp(p2,1))
					tt = 0;		
			cout << "z";
		}
		cout << "vvvvvvv";
		if (tt) {
			cout << "[" << p1 << "]";
			return p1;
		}
	}
}

string bigint::todec()
{
	bigint divc = 10000000000000000000;
	bigint a = *this;
	string res;
	while (a != 0) {
		uint64_t i = (a % divc).getuint64_t();
		string s = std::to_string(i);
		if(a != 0)
			while (s.size() < 19) {
				s = ' ' + s;
			}
		res = s+res;
		a = a / divc;
	}
	while (res.size() < 19) {
		res = '0' + res;
	}
	if (sign == -1)
		res += '-';
	return res;
}

bigint::~bigint()
{
}

void bigint::test()
{

	


	uint64_t ii[4];
	ii[0] = 0xffffffffffffffff;
	mulbig(ii[0], ii[0], ii[2], ii[3]);
	bigint a, b, c, d;
	
	a.vdata.resize(2);
	a.vdata[0] = 0xca4d6120f2408571;
	a.vdata[1] = 0x4e;
	a = 2;
	b = 1636;
	c.vdata.resize(2);
	c.vdata[0] = 0x02c7e14af6800000;
	c.vdata[1] = 0x152d;
	cout<<powmod(a, b, c)<<endl;

	cout << a<<endl;
	cout << a.todec()<<endl;
	a = 10;
	b = 20;

	for (int i = 0; i < 64; i++) {
		std::cout << getbitnum((uint64_t)1 << i)<<std::endl;
	}



	c = 1;
	int mmul = 50;
	int s = 0;
	for (int i = 64; i-- > 0;) {
		c = c * c;
		s = s * 2;
		if (mmul&((uint64_t)1 << i)) {
			c = c * a;
			s = s + 1;
		}
		//std::cout << std::dec <<s << std::endl;
		//std::cout <<std::dec<< i << ":" << c << std::endl;
	}
	std::cout << a + b << std::endl;

	a = 175;
	b = 235;
	c = 257;
	std::cout <<"powmod:"<< powmod(a,b,c) << std::endl;
	
	system("pause");
}

void bigint::opadd(const bigint &a)
{
	bool c = 0;
	for (int i = 0; i < a.vdata.size(); i++) {
		vdata[i] = addcarry(vdata[i], a.vdata[i], c);
	}
	size_t i = a.vdata.size();
	while (c) {
		vdata[i] = addcarry(vdata[i], 0, c);
		i++;
	}
}

void bigint::opsub(const bigint &a)
{
	bool c = 0;
	for (int i = 0; i < min(a.vdata.size(),vdata.size()); i++) {
		vdata[i] = subcarry(vdata[i], a.vdata[i], c);
	}
	size_t i = a.vdata.size();
	while (c) {
		vdata[i] = subcarry(vdata[i], 0, c);
		i++;
	}
}

void bigint::oprr()
{
	bool c = 0;
	for (size_t i = vdata.size() ; i-- > 0;) {
		vdata[i]=rrcarry(vdata[i],c);
	}
}

int bigint::opbigger(const bigint &a)const
{
	size_t N = vdata.size();

	if (vdata.size() > a.vdata.size()) {
		for (size_t i = a.vdata.size(); i < vdata.size(); i++) {
			if (vdata[i] != 0)
				return 1;
		}
		N = a.vdata.size();
	}
	if (vdata.size() < a.vdata.size()) {
		for (size_t i = vdata.size(); i < a.vdata.size(); i++) {
			if (a.vdata[i] != 0)
				return -1;
		}
		N = vdata.size();
	}
	for (size_t i = N; i-- > 0;) {
		if (vdata[i] > a.vdata[i])
			return 1;
		if (vdata[i] < a.vdata[i])
			return -1;
	}
	return 0;
}

int bigint::firstbitpos()const
{
	for (size_t i = vdata.size() ; i-- > 0;) {
		if (vdata[i] != 0)
			return static_cast<int>(getbitnum(vdata[i])+i*64);
	}
	return 0;
}

int bigint::lastbitpos() const
{
	for (size_t i = 0; i <vdata.size(); i++) {
		if (vdata[i] != 0)
			return static_cast<int>(getlbitnum(vdata[i]) + i * 64);
	}
	return -1;
}

void bigint::fit()
{
	for (size_t i = vdata.size(); i-- > 0;) {
		if (vdata[i] == 0)
			vdata.resize(i);
		else
			return;
	}
}

vector<uint64_t>& bigint::data()
{
	return vdata;
}
