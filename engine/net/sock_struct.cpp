#include "sock_struct.h"

unsigned int GetSockError()
{
#ifdef WIN32
	return WSAGetLastError();
#else
	return errno;
#endif // WIN32	
}

int DestroySocket(SOCKET socket)
{
#ifdef WIN32
	return closesocket(socket);
#else
	return close(socket);
#endif // WIN32	
}
