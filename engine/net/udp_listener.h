#pragma once

#include <string>
#include "sock_struct.h"
#include "sock.h"

using std::string;

class UDPListener
{
public:
	UDPListener(string ip, int port);
	~UDPListener();

	bool Init();
	SOCKET GetSocket() { return m_Socket; }
	int CloseSocket();

	Sock *GetSock() { return &m_Sock; }
private:
	string m_strIP;
	int m_nPort;
	SOCKET m_Socket;
	Sock m_Sock;
	char m_Data[8192];
public:
	struct sockaddr_in m_PeerAddress;
	SOCK_IO_CONTEXT m_IOContextReadFrom;
	CompletionEvent m_pCompletionEvent;	
	WSABUF  m_WsaBuf;
	DWORD m_dwFlags;
	DWORD m_dwBytes;
	socklen_t m_nFromlen;// = sizeof(sockaddr);
};
