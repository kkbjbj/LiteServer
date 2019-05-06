#pragma once

#include <queue>
#include <string>

#include "sock_struct.h"
#include "sock.h"

using std::string;

class TCPListener
{
public:
	TCPListener(string ip, int port);
	~TCPListener();
	bool Init();
	bool GetFuncAddr();
	int CloseSocket();
	
	string GetListenIP() { return m_strIP; }
	int GetListenPort() { return m_nPort; }
	SOCKET GetSocket() { return m_Socket; }

	AcceptBuff * NewAcceptBuff(SOCKET AcceptSocket);
	void DelAcceptBuff(AcceptBuff *buff);

	SOCK_IO_CONTEXT *NewIOContext(SOCKET AcceptSocket);
	void DelIOContext(SOCK_IO_CONTEXT *pIOContext);

	bool Listen() { return m_Sock.Listen(m_nPort); }
private:
	string m_strIP;
	int m_nPort;
	SOCKET m_Socket;
	Sock m_Sock;
public:	
	std::queue<AcceptBuff *> m_queAcceptBuff;
	std::queue<SOCK_IO_CONTEXT *> m_queIOContext;

	LPFN_ACCEPTEX m_pfnAcceptEx;
	LPFN_GETACCEPTEXSOCKADDRS m_pfnAcceptExSockAddr;
	CompletionEvent m_pCompletionEvent;

};
