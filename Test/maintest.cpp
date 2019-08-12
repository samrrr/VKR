
#include <winsock2.h>
#include "windows.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <thread>

#include "NCryptAuthRSA.h"
#include "NCryptHashSHA3.h"
#include "NCryptSymIDEA.h"
#include "NCryptSyncDiffHelm.h"
#include "NCryptConnectionWS.h"
#include "NCryptChannel.h"

using namespace std;

void savevector(string name,const vector<uint64_t> &v) {
	ofstream os(name, ios::binary);

	uint64_t sz = v.size();

	os.write(reinterpret_cast<const char*>(&sz), 8);
	os.write(reinterpret_cast<const char*>(v.data()), v.size()*8);

	os.close();
}
void loadvector(string name, vector<uint64_t> &v) {
	ifstream is(name, ios::binary);

	uint64_t sz;

	is.read(reinterpret_cast<char*>(&sz), 8);

	v.resize(sz);

	for (int i = 0; i<sz; i++)
		is.read(reinterpret_cast<char*>(&v[i]), 8);

	is.close();
}

void savevectorb(string name, const vector<uint8_t> &v) {
	ofstream os(name, ios::binary);

	uint64_t sz = v.size();

	os.write(reinterpret_cast<const char*>(v.data()), v.size());

	os.close();
}
void loadvectorb(string name, vector<uint8_t> &v) {
	ifstream is(name, ios::binary);

	uint8_t sz=0;


	is.seekg(0, is.end);
    ifstream::pos_type pos = is.tellg();
	is.seekg(0, is.beg);

	v.resize(pos);

    is.seekg(0, ios::beg);
    is.read(reinterpret_cast<char*>(v.data()), pos);


	is.close();
}

void testcl() {
	NCryptConnectionWS conn;
	if (!conn.connectToServer("109.172.10.205", 22))
		return;

	cout << "Cl conn" << endl;

	vector < uint64_t > vv= {1045,123456},vv1;
	uint64_t n1= 1045, n2= 123456;
	int res;
	int nn = 0;
	int nnr = 1000;
	while (1) {

		vv.resize(1000);
		for (int i = 0; i < vv.size(); i++, nn++)
			vv[i] = nn;
		res = conn.sendV(vv.data(), vv.size() * 8);

		int nnrst = nnr;
		//system("pause");
		do {
			//if (res)
			//	res = conn.recvV(vv1);

			if (res) {

				//for (int i = 0; i < vv1.size(); i++)
				//	cout << vv1[i] << endl;
				for (int i = 0; i < vv1.size(); i++, nnr++)
					if (vv1[i] != nnr)
						throw;
			}
			else
				throw;

		} while (nnrst + 1000 != nnr);
		cout << nnr << endl;

		//system("pause");
	}

}

void testsv() {

	NCryptConnectionWS conn;
	if (!conn.listenClients(22))
		return;

	vector < uint64_t > vv = { 1045,123456 }, vv1;
	uint64_t n1 = 1045, n2 = 123456;
	int res=-1;
	int nn = 0;
	int nnr = 0;
	while (1) {


		//system("pause");
		//res = conn.recvV(vv1);

		if (res) {

			//for (int i = 0; i < vv1.size(); i++)
			//	cout << vv1[i] << endl;
			for (int i = 0; i < vv1.size(); i++, nnr++) {
				if (vv1[i] != nnr)
					throw;
				vv1[i] += 1000;
			}
			res = conn.sendV(vv1.data(), vv1.size() * 8);
		}

		if (!res)
			throw;
		//system("pause");
	}
}

void testclM() {


	shared_ptr<NCryptAuthRSA> auth(new NCryptAuthRSA());
	vector<uint64_t> akey;
	loadvector("key1_public.bin", akey);
	auth->SetToken(akey);

	shared_ptr<NCryptSyncDiffHelm> sync(new NCryptSyncDiffHelm());
	sync->setsz(500);
	sync->setp(bigint("0x000d80b7ee5b8da724e173bc283eef9a010a85968b81f4fc998e9a848b349f9939d2591a655bd12a529e142e8c961bdbdf775d580ab1f1d953a21e5123c4fcfb"));

	NCryptSTR str;

	str.auth = auth;
	str.connect = make_shared<NCryptConnectionWS>();
	str.hash = make_shared<NCryptHashSHA3>();
	str.rg = make_shared<NRandomGenWin>();
	str.symm = make_shared<NCryptSymIDEA>();
	str.sync = sync;


	NCryptChannel channel(str);

	channel.Connect("127.0.0.1", 22);
	channel.synckey();
	vector<uint8_t> vf;
	loadvectorb("rrr.zip",vf);


	//channel.senddata(vf.data(), vf.size());
	vf.resize((vf.size() / 8 / 31+1) * 8 * 31);
	auto vf1 = vf;
	auth->SetPrivate(akey);
	{
		auto duration = std::chrono::system_clock::now().time_since_epoch();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		cout << "MMMMST:" << millis << endl;
	}
	for (int i = 0; i < vf.size() / 8 / 31; i++) {
		cout << i << " "<< vf.size() / 8 / 31 <<endl;
		vector<uint64_t> v1;
		vector<uint64_t> v2;
		v1.resize(31);
		v2.resize(31);
		memcpy_s(v1.data(), 31 * 8, vf.data()+i*8*31,8*31);
		str.auth->Crypt(v1,v2);
		memcpy_s(vf1.data() + i * 8 * 31, 31 * 8, v2.data() , 8 * 31);
	}
	{
		auto duration = std::chrono::system_clock::now().time_since_epoch();
		auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
		cout << "MMMMST:" << millis << endl;
	}

}

void testsvM() {

	shared_ptr<NCryptAuthRSA> auth(new NCryptAuthRSA());
	vector<uint64_t> akey;
	loadvector("key1_private.bin", akey);
	auth->SetPrivate(akey);

	shared_ptr<NCryptSyncDiffHelm> sync(new NCryptSyncDiffHelm());
	sync->setsz(500);
	//sync->setp(bigint("59850144754603371887"));

	NCryptSTR str;

	str.auth = auth;
	str.connect = make_shared<NCryptConnectionWS>();
	str.hash = make_shared<NCryptHashSHA3>();
	str.rg = make_shared<NRandomGenWin>();
	str.symm = make_shared<NCryptSymIDEA>();
	str.sync = sync;

	NCryptChannel channel(str);

	channel.Listen(22);
	channel.synckey();
	vector<uint8_t> bvec;
	
	channel.recvdata(bvec);

	auto duration = std::chrono::system_clock::now().time_since_epoch();
	auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	cout << "MMMMEND:" << millis << endl;

	savevectorb("ttt1.zip", bvec);
	int i;
	cin >> i;

}

void enct() {
	vector<uint8_t> v;
	loadvectorb("t.txt", v);
	v.resize((v.size() + 7) / 8 * 8);

	NCryptSymIDEA idea;
	idea.SetKey(vector<uint64_t>() = { 0x1234123a3e569854,0xfa5ff4a014567215 });

	idea.Encode(v.data(), v.data(), v.size());

	savevectorb("t1.txt", v);

	loadvectorb("t.txt", v);
	v.resize((v.size() + 7) / 8 * 8);

	for (int i = 0; i < v.size() / 8; i++) {

		idea.Encode(v.data() + i * 8, v.data() + i * 8, 8);
		if(i+1 < v.size() / 8)
		for (int r = 0; r < 8; r++)
			v[(i + 1) * 8 + r] ^= v[i * 8 + r];
	}

	savevectorb("t2.txt", v);


}

int main() {

	{
		bigint n("163323987240443679101402070093049155030989439806917519173580070791569227728932850358498862854399351423733697660534800194492724828721314980248259450358792069235991826588944200440687094136669506349093691768902440555341493237296555254247379422702221515929837629813600812082006124038089463610239236157651252180491");
		testp(n,100);

	}
	/*
	{
		NCryptAuthRSA auth;

		vector<uint64_t> k1, k2;

		auth.Generate(1024, k1, k2, NBASICRG);

		savevector("key2_private.bin", k1);
		savevector("key2_public.bin", k2);

		auth.Generate(1024, k1, k2, NBASICRG);

		savevector("key1_private.bin", k1);
		savevector("key1_public.bin", k2);

	}
	*/
	{
		std::thread th1, th2;
		th1 = thread(testsvM);
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		th2 = thread(testclM);

		th1.join();
		th2.join();
	}
	return 0;
	enct();
	/*
	{

		NCryptAuthRSA auth;

		vector<uint64_t> k1, k2;

		//auth.Generate(129, k1, k2, NBASICRG);

		savevector("key2_private.bin", k1);
		savevector("key2_public.bin", k2);
	}
	//fibonacci_init(1, 1);
	//cout << fibonacci_current() << endl;
	//testcl();
	std::thread th1, th2;
	th1 = thread(testsvM);
	std::this_thread::sleep_for(std::chrono::milliseconds(100));
	th2 = thread(testclM);

	th1.join();
	th2.join();
	vector<uint64_t> vv;

	for (unsigned int i = 0; i <vv.size()*8; i++)
	{
		printf("%.2x", (unsigned char)(vv[i/8]>>(8*(i%8))));
	}

	vector<uint8_t> data(256);
	for (int i = 0; i < 256; i++)
		data[i] = i;

	for (int i = 0; i < 1000;) {
		NCryptSyncDiffHelm he1, he2;
		vector<uint64_t> arr1, arr2;

		he1.setsz(514);
		he2.setsz(514);
		he1.step1(arr1, arr2, 0, NBASICRG);
		he2.step2(arr2, arr1, 0, NBASICRG);
		he1.step1(arr1, arr2, 1, NBASICRG);
		arr1=he1.getResult();
		arr2=he2.getResult();
		if (arr1.size() != arr2.size())
			throw;
		for (int i = 0; i < arr1.size(); i++) {
			if (arr1[i] != arr2[i])
				throw;
		}
		cout << bigint(arr1,1)<<endl;
	}
	*/
	NCryptSymIDEA symidea;

	symidea.SetKey(vector<uint64_t>() = { 0,0 });

	uint64_t m1 = 0x0003000200010000,m2;

	symidea.Encode(&m1, &m2, 8);

	symidea.Decode(&m2, &m1, 8);

	NCryptAuthRSA auth;

	vector<uint64_t> k1, k2;

	auth.Generate(512, k1, k2, NBASICRG);

	savevector("key3_private.bin", k1);
	savevector("key3_public.bin", k2);

	vector<uint64_t> arrh;
	arrh.resize(5);
	arrh[0] = 1;
	arrh[1] = 2;
	arrh[2] = 3;
	arrh[3] = 4;
	arrh[4] = 5;

	vector<uint64_t> 
		arrres;

	auth.SetPrivate(k1);
	auth.Crypt(arrh,  arrres);

	auth.SetToken(k2);
	//arrh[0] = 0;
	cout<<auth.Test(arrres, arrh);
		
		
	//auto res=randomPrime(100, std::shared_ptr<NRandomGen>(static_cast<NRandomGen*>(new NRandomGenWin())));
	//cout << res.todec()<<endl;

	system("pause");
}