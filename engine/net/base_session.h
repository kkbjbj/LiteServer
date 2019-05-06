#pragma once

#include <string.h>
#include "sock_struct.h"

class BaseSession
{
public:
	BaseSession() 
	{ 
		m_bAlive = false;
		m_nPort = 0;
		m_Socket = INVALID_SOCKET;
		m_nSockType = SOCK_NONE;
		m_nSessionID = 0;
	}
	~BaseSession() {}

	string GetIP() { return m_strIP; }	
	void SetIP(string ip) { m_strIP = ip; }
	int GetPort() { return m_nPort; }
	void SetPort(int port) { m_nPort = port; }
	int GetSessionID() { return m_nSessionID; }
	void SetSessionID(int SessionID) { m_nSessionID = SessionID; }

	struct sockaddr_in *GetAddr() { return &m_Address; }
	void SetAddr(struct sockaddr_in *addr) { memcpy(&m_Address, addr, sizeof(struct sockaddr_in)); }
	uint64_t GetSessionKey() { uint64_t key = m_Address.sin_port; key <<= 32; key |= m_Address.sin_addr.s_addr; return key; }

	SOCK_TYPE GetSockType() { return m_nSockType; }
	SOCKET GetSocket() { return m_Socket; }
	Sock *GetSock() { return &m_Sock; }
	
	virtual void Init(SOCKET Socket, SOCK_TYPE SockType) = 0;
	virtual bool Send(const char* Data, unsigned int Len) = 0;
	virtual bool DoSend() = 0;

	bool IsAlive() { return m_bAlive; }
	void SetAlive(bool alive) { m_bAlive = alive; }
protected:
	bool m_bAlive;
	char m_Data[8192];
	string m_strIP;
	int m_nPort;
	SOCKET m_Socket;
	SOCK_TYPE m_nSockType;
	struct sockaddr_in m_Address;
	Sock m_Sock;
	int m_nSessionID;

};
