#include "NCryptConnectionWS.h"

inline NCryptConnectionWS::~NCryptConnectionWS() {
	if (isStartup)
		WSACleanup();
}

