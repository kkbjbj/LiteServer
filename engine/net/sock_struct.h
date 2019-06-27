#pragma once

#ifdef _WIN32
	#include <winsock2.h>
	#include <mswsock.h>
	#include <WS2tcpip.h>
#else
	#include <sys/epoll.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <netdb.h>
	#include <arpa/inet.h>
	#include<errno.h>
	#include <fcntl.h>
	#include <unistd.h>
#endif // _WIN32	

#include <string.h>
#include <stdio.h>
#include <string>
	

#ifndef _WIN32
	typedef void *PVOID;
	typedef uint32_t SOCKET;
	typedef uint32_t DWORD;
	typedef uint32_t SOCKHANDLE;
	typedef void *LPFN_CONNECTEX;
	typedef void *LPFN_ACCEPTEX;
	typedef void *LPFN_GETACCEPTEXSOCKADDRS;

	#define SOCKET_ERROR -1
	#define INVALID_SOCKET -1

	typedef struct
	{
		void *Pointer;
	} WSAOVERLAPPED;

	typedef struct
	{
    	uint32_t len;
    	char *buf;
	} WSABUF;
#else
	typedef  HANDLE SOCKHANDLE;
#endif

unsigned int GetSockError();
int DestroySocket(SOCKET);

class TCPListener;

enum SOCK_IO_TYPE
{
	IO_NONE = 0,
	IO_CONNECT,
	IO_ACCEPT,
	IO_READ,
	IO_READFROM,
	IO_WRITE,
};

enum SOCK_TYPE
{
	SOCK_NONE = 0,
	SOCK_LISTEN,
	SOCK_CONNECT,
	SOCK_ACCEPTED,
	SOCK_UDP,
};

struct CompletionEvent
{
	CompletionEvent() :SockType(SOCK_NONE), Socket(SOCKET_ERROR), Param(NULL) {}
	CompletionEvent(SOCK_TYPE sockType, SOCKET socket, void *param):SockType(sockType),Socket(socket),Param(param) {}
	SOCK_TYPE SockType;
	SOCKET Socket;
	void *Param;
};

struct SOCK_IO_CONTEXT
{
	SOCK_IO_CONTEXT()
	{
		OperType = IO_NONE;
		Param = NULL;
		memset(&olOverlap, 0, sizeof(olOverlap));
	}
	SOCKET Socket;
	SOCK_IO_TYPE OperType;
	WSAOVERLAPPED olOverlap;
	void *Param;
};

struct AcceptBuff
{
	SOCKET AcceptSocket;
	TCPListener *pListener;
	char Buff[128];
};


class NetEventCallBack
{
public:
	virtual bool OnAddConnection(int SessionID, SOCKET Sock, std::string IP, int Port) = 0;
	virtual bool OnConnectionDrop(int SessionID, SOCKET Sock) = 0;
	virtual bool OnConnected(int SessionID, SOCKET Sock) = 0;
	virtual bool OnConnectFail(int SessionID, SOCKET Sock) = 0;	
	virtual bool OnData(int SessionID, SOCKET Sock, char *Data, int size) = 0;
	virtual bool OnUpdate() = 0;
};


