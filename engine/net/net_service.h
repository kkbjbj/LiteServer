#pragma once

#include <map>
#include <queue>
#include <vector>
#include <thread>
#include "sock_struct.h"

#define MAX_SOCK_EVENT 20000

class TCPListener;
class UDPListener;
class BaseSession;
class TCPSession;
class TCPConnector;

class NetService
{
public:
	NetService();
	~NetService();

public:
	int NewSessionID();
	void FreeSessionID(int SessionID);
	void InitSession();

	bool Init(NetEventCallBack *pCallBack);
	bool Start();
	bool Stop();
	bool CleanUp();
	void ThreadProc();	

	bool AddTCPListener(TCPListener * listener);
	bool AddUDPListener(UDPListener * listener);
	bool AddTCPConnector(TCPConnector * pConnector);
	bool PostAccept(TCPListener * listener);
	bool DoAccpet(AcceptBuff * buff, DWORD dwIoSize);
	bool PostConnect(TCPConnector *pTCPConnector);
	bool DoConnect(TCPConnector *pTCPConnector, DWORD dwIoSize);
	bool PostRead(TCPSession *pTcpSession);
	bool PostReadFrom(UDPListener * listener);
	bool DoRead(TCPSession *pTcpSession, DWORD dwIoSize);
	bool DoReadFrom(UDPListener * listener, DWORD dwIoSize);
	void SendWorker();

	void AddSession(BaseSession *pSession);
	void DelSession(BaseSession *pSession);
	int GetUDPSessionID(struct sockaddr_in *pAddress);
	void SetUDPSessionID(struct sockaddr_in *pAddress, int SessionID);

	void SetNetEventCallBack(NetEventCallBack *pCallBack) { m_pNetEventCallBack = pCallBack; }
	bool KickSession(int SessionID, SOCKET socket);
	bool OnSessionDrop(BaseSession *pSession);
private:
#ifndef WIN32
	struct epoll_event m_Events[MAX_SOCK_EVENT];
#endif
	SOCKHANDLE m_ServiceHandle;
	NetEventCallBack *m_pNetEventCallBack;
	std::vector<std::thread> m_Threads;
	std::queue<int> m_queSessionID;
	std::map<uint64_t, int> m_mapUDPSessions;
	BaseSession * m_Sessions[MAX_SOCK_EVENT];
	bool m_bRunning;
};
