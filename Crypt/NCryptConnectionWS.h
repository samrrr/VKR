#pragma once
#include "NCryptBase.h"

#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>

#pragma comment (lib, "Ws2_32.lib")

class NCryptConnectionWS: public NCryptConnection {
private:
	static WSADATA wsaData;
	static int isStartup;

	struct addrinfo haddr;
	SOCKET ConnectSocket = INVALID_SOCKET;

	bool WSinit() {
		if (isStartup)
			return 1;
		int iResult;
		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			return 0;
		}
		isStartup = 1;
		return 1;
	}
public:
	virtual bool connectToServer(const char* ip, uint16_t port) {
		if (!WSinit())
			return 0; 

		int iResult;

		ZeroMemory(&haddr, sizeof(haddr));
		haddr.ai_family = AF_UNSPEC;
		haddr.ai_socktype = SOCK_STREAM;
		haddr.ai_protocol = IPPROTO_TCP;

		char portbuf[10];
		_itoa_s(port,portbuf,10);

		struct addrinfo *result = NULL;

		iResult = getaddrinfo(ip, portbuf, &haddr, &result);
		if (iResult != 0)
			return 0;

		for (auto ptr = result; ptr != NULL; ptr = ptr->ai_next) {

			// Create a SOCKET for connecting to server
			ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype,
				ptr->ai_protocol);
			if (ConnectSocket == INVALID_SOCKET) {
				freeaddrinfo(result);
				return 0;
			}

			// Connect to server.
			iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
			if (iResult == SOCKET_ERROR) {
				closesocket(ConnectSocket);
				ConnectSocket = INVALID_SOCKET;
				continue;
			}
			break;
		}

		freeaddrinfo(result);

		if (ConnectSocket == INVALID_SOCKET) {
			return 0;
		}

		//u_long iMode = 1;
		//iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
		//if (iResult != NO_ERROR)
		//	return 0;

		return 1;
	}
	virtual bool listenClients(uint16_t port) {
		if (!WSinit())
			return 0;

		SOCKET ListenSocket = INVALID_SOCKET;
		int iResult;

		struct addrinfo *result = NULL;
		struct addrinfo hints;


		char portbuf[10];
		_itoa_s(port, portbuf, 10);

		iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
		if (iResult != 0) {
			printf("WSAStartup failed with error: %d\n", iResult);
			return 0;
		}

		ZeroMemory(&hints, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_protocol = IPPROTO_TCP;
		hints.ai_flags = AI_PASSIVE;

		// Resolve the server address and port
		iResult = getaddrinfo(NULL, portbuf, &hints, &result);
		if (iResult != 0) {
			printf("getaddrinfo failed with error: %d\n", iResult);
			return 0;
		}

		// Create a SOCKET for connecting to server
		ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
		if (ListenSocket == INVALID_SOCKET) {
			printf("socket failed with error: %ld\n", WSAGetLastError());
			freeaddrinfo(result);
			return 0;
		}

		// Setup the TCP listening socket
		iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			printf("bind failed with error: %d\n", WSAGetLastError());
			freeaddrinfo(result);
			closesocket(ListenSocket);
			return 0;
		}

		freeaddrinfo(result);

		iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			return 0;
		}

		// Accept a client socket
		ConnectSocket = accept(ListenSocket, NULL, NULL);
		if (ConnectSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			return 0;
		}


		struct timeval tv;

		tv.tv_sec = 30;
		//setsockopt(ConnectSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));
		//setsockopt(ConnectSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&tv, sizeof(struct timeval));

		//u_long iMode = 1;
		//iResult = ioctlsocket(ConnectSocket, FIONBIO, &iMode);
		//if (iResult != NO_ERROR)
		//	return 0;

		return 1;
	}
	virtual bool sendV(void* sendbuf, uint64_t size) {
		int offs = 0;
		int iResult;
		while (offs < size) {
			iResult = send(ConnectSocket, reinterpret_cast<const char *>(sendbuf) + offs, size- offs, 0);
			if (iResult == SOCKET_ERROR) {
				return 0;
			} 
			if (iResult > 0)
				offs += iResult;
		}

		return 1;
	}
	virtual bool recvV(void* recvbuf, uint64_t size) {
		int recvbufdtoffset = 0;
		int iResult;
		do {
			iResult = recv(ConnectSocket, reinterpret_cast<char *>(recvbuf) + recvbufdtoffset, size - recvbufdtoffset, 0);
			if (iResult == 0) {
				return 0;
			}
			if (iResult < 0) {
				return 0;
			}
			recvbufdtoffset += iResult;
		} while (recvbufdtoffset<size);
		
		return 1;
	}
	virtual void disconnectConn() {
		closesocket(ConnectSocket);
		ConnectSocket = INVALID_SOCKET;
	}
	__declspec(dllexport) ~NCryptConnectionWS();
};
WSADATA NCryptConnectionWS::wsaData;
int NCryptConnectionWS::isStartup = 0;


