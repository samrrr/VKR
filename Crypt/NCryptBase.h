// Базовые абстрактные классы.

#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include "bigint.h"

// Абстрактный базовый класс генератора случайных чисел
class NRandomGen {
public:
	// Генерация случайного числа
	virtual bool GetRandom(void *, int64_t) = 0;
};

// Абстрактный класс симметричного шифрования
class NCryptSymmetric {
public:
	// Получить шаг данных в байтах
	virtual const uint64_t GetDataScale() = 0;

	// Получить размер ключа данных в байтах
	virtual const uint64_t GetKeySize() = 0;

	// Установить ключ
	virtual bool SetKey(std::vector<uint64_t>) = 0;

	// Зашифровать
	virtual bool Encode(void*, void*, uint64_t) = 0;

	// Расшифровать
	virtual bool Decode(void*, void*, uint64_t) = 0;

	virtual __declspec(dllexport)  ~NCryptSymmetric();
};

// Абстрактный класс ассимметричного шифрования
class NCryptAsymmetric {
public:
	// Получить шаг данных в байтах
	virtual const uint64_t GetDataScale() = 0;

	// Установить приватный ключ
	virtual bool SetPrivateKey(std::vector<uint64_t>) = 0;

	// Установить публичный ключ
	virtual bool SetPublicKey(std::vector<uint64_t>) = 0;

	// Зашифровать
	virtual bool Crypt(void*, uint64_t, void*, uint64_t) = 0;

	// Расшифровать
	virtual bool Encrypt(void*, uint64_t) = 0;

	virtual __declspec(dllexport)  ~NCryptAsymmetric();
};

// Абстрактный класс аутентификации(подписи)
class NCryptAutentification {
public:
	// Максимальный размер подписываемого сообщения
	virtual int MaxMSize() = 0;

	// Установить приватный ключ
	virtual bool SetPrivate(std::vector<uint64_t>) = 0;

	// Установить публичный ключ
	virtual bool SetToken(std::vector<uint64_t>) = 0;

	// Сравнение зашифрованного хеша с хешом сообщения
	virtual bool Test(std::vector<uint64_t> ,std::vector<uint64_t>) = 0;

	// Генерирование ключей
	virtual bool Generate(int64_t, std::vector<uint64_t>&, std::vector<uint64_t>&, std::shared_ptr<NRandomGen>) = 0;

	// Шифрование хеша 
	virtual bool Crypt(std::vector<uint64_t>,std::vector<uint64_t> &) = 0;

	virtual __declspec(dllexport)  ~NCryptAutentification();
};

// Абстрактный класс хеш функции
class NCryptHash {
public:
	// Хеш от данных
	virtual bool Hash(void*, uint64_t, std::vector<uint64_t>&) = 0;

	// Получение размера хеша
	virtual int HashSize() = 0;

	virtual __declspec(dllexport) ~NCryptHash();
};

// Абстрактный класс синхронизации общего секрета
class NCryptSync {
public:
	// Выполнение шага для первого участника
	virtual int step1(const vector<uint64_t> &, vector<uint64_t> &, int, std::shared_ptr<NRandomGen>rg) = 0;

	// Выполнение шага для второго участника
	virtual int step2(const vector<uint64_t> &, vector<uint64_t> &, int, std::shared_ptr<NRandomGen>rg) = 0;

	// Размер входных данных для шага
	virtual int stepsize(int type, int step) = 0;

	// Получение общего секрета
	virtual vector<uint64_t> getResult() = 0;

	// Количество случайных бит в секрете
	virtual int randomBitsCount() = 0;

	virtual __declspec(dllexport) ~NCryptSync();
};

// Абстрактный класс незащищённого соединения 
class NCryptConnection {
public:
	// Соединение с сервером
	virtual bool connectToServer(const char* ip, uint16_t port) = 0;

	// Ожидание клиентов
	virtual bool listenClients(uint16_t port) = 0;

	// Посылка данных
	virtual bool sendV(void*, uint64_t) = 0;

	// Приём данных
	virtual bool recvV(void*, uint64_t) = 0;

	// Обрыв соединения
	virtual void disconnectConn() = 0;

	// Генерация случайного числа             
	virtual __declspec(dllexport) ~NCryptConnection();
};


#ifdef _WIN64
// Генератор случайных байтов
class NRandomGenWin :public NRandomGen {
	__declspec(dllexport) bool GetRandom(void *, int64_t);
};
#define NBASICRG std::shared_ptr<NRandomGen>(static_cast<NRandomGen*>(new NRandomGenWin()))
#endif


// Генератор случайного числа заданного размера
extern __declspec(dllexport) bigint randomNumberGen(std::shared_ptr<NRandomGen> rg, int size, int addbit = 1);

// Генератор случайного простого числа заданного размера
extern __declspec(dllexport) bigint randomPrime(int size, std::shared_ptr<NRandomGen>rg);

// Генератор числа p заданного размера такого, что p и (p-1)/2 простые
extern __declspec(dllexport) bigint randomExtraPrime(int size, std::shared_ptr<NRandomGen>rg);
