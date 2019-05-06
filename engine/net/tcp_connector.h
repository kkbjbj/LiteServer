#pragma once

#include <string>
#include "sock_struct.h"
#include "tcp_session.h"
#include "sock.h"

using std::string;

class TCPConnector
{
public:
	TCPConnector(string ip, int port);
	~TCPConnector();

	SOCKET GetSocket() { return m_Socket; }
	string GetIP() { return m_strIP; }
	int GetPort() { return m_nPort; }
	void SetConnected() { m_Connected = true; }
	bool IsConnected() { return m_Connected; }

	bool Init();
	bool GetFuncAddr();
	int CloseSocket();
	Sock *GetSock() { return m_TcpSession.GetSock(); }

	TCPSession *GetSession() { return &m_TcpSession; }
private:
	string m_strIP;
	int m_nPort;
	SOCKET m_Socket;
	bool m_Connected;
public:
	struct sockaddr_in m_ServerAddress;	
	SOCK_IO_CONTEXT m_IOContextConnect;
	TCPSession m_TcpSession;

	LPFN_CONNECTEX m_lpfnConnectEx;
	CompletionEvent m_pCompletionEvent;
};

