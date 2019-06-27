#include "sock_struct.h"

unsigned int GetSockError()
{
#ifdef _WIN32
	return WSAGetLastError();
#else
	return errno;
#endif // _WIN32	
}

int DestroySocket(SOCKET socket)
{
#ifdef _WIN32
	return closesocket(socket);
#else
	return close(socket);
#endif // _WIN32	
}
