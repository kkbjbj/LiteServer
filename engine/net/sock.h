#pragma once

#include <string>
#include "sock_struct.h"
using std::string;

class Sock
{
public:
	Sock() {}
	~Sock() {}

	string GetIP() { return m_strIP; }	
	int GetPort() { return m_Address.sin_port; }
	SOCKET GetSocket() { return m_Socket; }
	void SetSocket(SOCKET Socket);
	bool Create(int Family, int Type);
	
	bool SetNonBlock();
	bool SetReUseAddr();
	bool Bind(string IP, int Port);
	bool Listen(int MaxConn = 1024);                    
	int Send(const char* Data, unsigned int Len);
	int SendTo(const char* Data, unsigned int Len, struct sockaddr *PeerAddr);
	int SendTo(const char* Data, unsigned int Len, string IP, int port);
	int Close();

private:
	SOCKET m_Socket;
	string m_strIP;
	struct sockaddr_in m_Address;
};
