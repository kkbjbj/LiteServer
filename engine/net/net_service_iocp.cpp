
#include "net_service.h"
#include "tcp_listener.h"
#include "udp_listener.h"
#include "tcp_connector.h"
#include "tcp_session.h"
#include "udp_session.h"

#ifdef WIN32



bool NetService::Init(NetEventCallBack *pCallBack)
{
	InitSession();
	m_pNetEventCallBack = pCallBack;
	WSADATA wsaData = {0};
	return 0 == WSAStartup(MAKEWORD(2, 2), &wsaData);
}

bool NetService::Start()
{
	if (NULL == m_pNetEventCallBack)
	{
		printf("m_pNetEventCallBack is null Call SetNetEventCallBack first.\n");
		return false;
	}
	m_ServiceHandle = CreateIoCompletionPort((HANDLE)INVALID_HANDLE_VALUE, NULL, 0, 0);
	if (NULL == m_ServiceHandle)
	{
		return false;
	}

	m_bRunning = true;

	SYSTEM_INFO si;
	GetSystemInfo(&si);
	size_t Processors = si.dwNumberOfProcessors + 2;
	for (size_t i = 0; i < 1; i++)
	{
		m_Threads.push_back(std::thread(std::bind(&NetService::ThreadProc, this)));
	}

	for (size_t i = 0; i < 1; i++)
	{
		m_Threads.push_back(std::thread(std::bind(&NetService::SendWorker, this)));
	}

	return true;
}

bool NetService::Stop()
{
	m_bRunning = false;
	for (size_t i = 0; i < m_Threads.size(); i++)
	{
		PostQueuedCompletionStatus(m_ServiceHandle, 0, (DWORD)NULL, NULL);
	}

	for (size_t i = 0; i < m_Threads.size(); i++)
	{
		m_Threads[i].join();
	}


	return true;
}

bool NetService::CleanUp()
{
	WSACleanup();
	return true;
}

void NetService::ThreadProc()
{
	DWORD dwIoSize = 0;
	OVERLAPPED *pOverlapped = NULL;
	CompletionEvent *pCompletionKey = NULL;
	ULONG_PTR pIoBuffer = NULL;

	while (m_bRunning)
	{
		m_pNetEventCallBack->OnUpdate();
		BOOL bOK = ::GetQueuedCompletionStatus(m_ServiceHandle, &dwIoSize, (PULONG_PTR)&pCompletionKey, &pOverlapped, 1);
		if (pCompletionKey == NULL || pOverlapped == NULL)
		{
			DWORD dwErr = GetSockError();
			if (WAIT_TIMEOUT == dwErr)
			{
				continue;
			}
			else
			{
				break;
			}
			
		}
			
		
		if (!bOK)
		{
			DWORD dwErr = GetSockError();
			printf("GetQueuedCompletionStatus failed with error: %u\n", dwErr);
			//if (IO_READ == pContext->OperType)
			//{
			//	TCPSession *pTcpSession = (TCPSession *)pContext->Param;
			//	OnSessionDrop(pTcpSession);
			//}

			//if (IO_CONNECT == pContext->OperType)
			//{
			//	TCPConnector *pTCPConnector = (TCPConnector *)pCompletionKey->Param;
			//	OnSessionDrop(pTCPConnector->GetSession());
			//	printf("Connect failed with error: %u\n", dwErr);
			//}
			continue;
		}
		SOCK_IO_CONTEXT *pContext = (SOCK_IO_CONTEXT *)pOverlapped->Pointer;		
		switch (pContext->OperType)
		{
		case IO_ACCEPT:
		{
			AcceptBuff * buff = (AcceptBuff *)pContext->Param;
			DoAccpet(buff, dwIoSize);			
			PostAccept(buff->pListener);
			buff->pListener->DelIOContext(pContext);
		}
		break;
		case IO_READ:
		{
			TCPSession *pTcpSession = (TCPSession *)pContext->Param;
			DoRead(pTcpSession, dwIoSize);
		}
		break;
		case IO_READFROM:
		{
			UDPListener *pUDPListener = (UDPListener *)pContext->Param;
			DoReadFrom(pUDPListener, dwIoSize);
			PostReadFrom(pUDPListener);
		}
		break;
		
		case IO_CONNECT:
		{
			TCPConnector *pTCPConnector = (TCPConnector *)pCompletionKey->Param;
			DoConnect(pTCPConnector, dwIoSize);
		}
		break;
		default:
			break;
		}
	}
}

bool NetService::AddTCPListener(TCPListener * listener)
{
	CreateIoCompletionPort((HANDLE)listener->GetSocket(), m_ServiceHandle, (ULONG_PTR)&listener->m_pCompletionEvent, 0);
	listener->Listen();

	bool ret = PostAccept(listener);
	if (ret)
	{
		for (size_t i = 0; i < 10; i++)
		{
			PostAccept(listener);
		}
	}

	return ret;
}

bool NetService::AddUDPListener(UDPListener * listener)
{
	CreateIoCompletionPort((HANDLE)listener->GetSocket(), m_ServiceHandle, (ULONG_PTR)&listener->m_pCompletionEvent, 0);

	return  PostReadFrom(listener);
}

bool NetService::AddTCPConnector(TCPConnector * pConnector)
{
	pConnector->GetSession()->SetAlive(false);
	int session_id = NewSessionID();
	if (0 == session_id)
	{
		printf("no sessionid left\n");
		return false;
	}
	pConnector->GetSession()->SetSessionID(session_id);

	CreateIoCompletionPort((HANDLE)pConnector->GetSocket(), m_ServiceHandle, (ULONG_PTR)&pConnector->m_pCompletionEvent, 0);

	DWORD dwBytesSent = 0;
	BOOL bResult = pConnector->m_lpfnConnectEx(pConnector->GetSocket(),
		(sockaddr *)&pConnector->m_ServerAddress, sizeof(pConnector->m_ServerAddress),
		NULL, 0,
		&dwBytesSent,
		&(pConnector->m_IOContextConnect.olOverlap));
	if (bResult == FALSE) 
	{
		int x = GetSockError();
		if (WSA_IO_PENDING != x)
		{
			printf("ConnectEx failed with error: %u\n", x);
			FreeSessionID(session_id);
			return false;
		}
	}

	return true;
}

bool NetService::PostAccept(TCPListener * listener)
{
	SOCKET AcceptSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (AcceptSocket == INVALID_SOCKET) {
		printf("Create accept socket failed with error: %u\n", GetSockError());
		return false;
	}

	SOCK_IO_CONTEXT *pContext = listener->NewIOContext(AcceptSocket);
	AcceptBuff * pBuff = (AcceptBuff *)pContext->Param;

	DWORD dwBytes = 0;
	BOOL bRetVal = listener->m_pfnAcceptEx(listener->GetSocket(), AcceptSocket,
		pBuff->Buff, 0,
		sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16,
		&dwBytes, &pContext->olOverlap);
	if (bRetVal == FALSE) 
	{
		int x = GetSockError();
		if (WSA_IO_PENDING != x)
		{
			printf("AcceptEx failed with error: %u\n", x);
			closesocket(AcceptSocket);
			return false;
		}
		
	}

	return true;
}

bool NetService::DoAccpet(AcceptBuff * pBuff, DWORD dwIoSize)
{
	sockaddr_in* ClientAddr = NULL;
	sockaddr_in* LocalAddr = NULL;
	int remoteLen = sizeof(sockaddr_in);
	int localLen = sizeof(sockaddr_in);

	pBuff->pListener->m_pfnAcceptExSockAddr(pBuff->Buff, 0, sizeof(sockaddr_in) + 16, sizeof(sockaddr_in) + 16, (LPSOCKADDR*)&LocalAddr, &localLen, (LPSOCKADDR*)&ClientAddr, &remoteLen);

	char ip[128] = { 0 };
	const char * pip = NULL;
	inet_ntop(AF_INET, (void *)&ClientAddr->sin_addr.s_addr, ip, remoteLen);
	printf("Accept %s:%d \n", ip, ntohs(ClientAddr->sin_port));

	int session_id = NewSessionID();
	if (0 == session_id)
	{
		DestroySocket(pBuff->AcceptSocket);
		return false;
	}

	TCPSession *pTcpSession = new TCPSession();
	pTcpSession->SetSessionID(session_id);
	pTcpSession->Init(pBuff->AcceptSocket, SOCK_ACCEPTED);
	pTcpSession->SetAddr(ClientAddr);
	pTcpSession->SetIP(ip);
	pTcpSession->SetPort(ntohs(ClientAddr->sin_port));
	pTcpSession->SetAlive(true);
	
	HANDLE hTemp = CreateIoCompletionPort((HANDLE)pBuff->AcceptSocket, m_ServiceHandle, (ULONG_PTR)&pTcpSession->m_CompletionEvent, 0);

	if (NULL == hTemp)
	{
		delete pTcpSession;
		DestroySocket(pBuff->AcceptSocket);
		FreeSessionID(session_id);
		printf("DoAccpet failed with error: %u\n", GetSockError());
		return false;
	}

	AddSession(pTcpSession);

	m_pNetEventCallBack->OnAddConnection(pTcpSession->GetSessionID(), pTcpSession->GetSocket(), pTcpSession->GetIP(), pTcpSession->GetPort());
	PostRead(pTcpSession);

	return true;
}

bool NetService::DoConnect(TCPConnector *pTCPConnector, DWORD dwIoSize)
{
	pTCPConnector->SetConnected();
	AddSession(&pTCPConnector->m_TcpSession);
	m_pNetEventCallBack->OnConnected(pTCPConnector->m_TcpSession.GetSessionID(), pTCPConnector->m_TcpSession.GetSocket());
	bool result = PostRead(&pTCPConnector->m_TcpSession);
	if(!result)
	{
		pTCPConnector->CloseSocket();
		OnSessionDrop(pTCPConnector->GetSession());
		printf("DoConnect error: %d\n",GetSockError());
	}
	pTCPConnector->GetSession()->SetAlive(true);
	return result;
}

bool NetService::PostRead(TCPSession *pTcpSession)
{

	int nBytesRecv = WSARecv(pTcpSession->GetSocket(), &(pTcpSession->m_WsaBuf), 1, &pTcpSession->m_dwBytes, &pTcpSession->m_dwFlags, &(pTcpSession->m_IOContextRead.olOverlap), NULL);

	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != GetSockError()))
	{
		printf("WSARecv failed with error: %u\n", GetSockError());
		return false;
	}

	return true;

}

bool NetService::PostReadFrom(UDPListener * listener)
{
	
	int nBytesRecv = WSARecvFrom(listener->GetSocket(), &listener->m_WsaBuf, 1, &listener->m_dwBytes, &listener->m_dwFlags, (struct sockaddr*)&listener->m_PeerAddress, &listener->m_nFromlen, &(listener->m_IOContextReadFrom.olOverlap), NULL);

	if ((SOCKET_ERROR == nBytesRecv) && (WSA_IO_PENDING != GetSockError()))
	{
		printf("WSARecvFrom failed with error: %u\n", GetSockError());
		return false;
	}

	return true;
}

bool NetService::DoRead(TCPSession *pTcpSession, DWORD dwIoSize)
{
	char str[8192] = {0};
	for (size_t i = 0; i < dwIoSize; i++)
	{
		str[i] = pTcpSession->m_WsaBuf.buf[i];
	}
	
	if (0 == dwIoSize)
	{
		printf("Socket Closed by peer %d \n", pTcpSession->GetSocket());
		OnSessionDrop(pTcpSession);
		return true;
	}

	//printf("%d Recv Tcp Data %s\n", pTcpSession->GetSocket(), str);


	m_pNetEventCallBack->OnData(pTcpSession->GetSessionID(), pTcpSession->GetSocket(), str, dwIoSize);

	PostRead(pTcpSession);
	return true;
}

bool NetService::DoReadFrom(UDPListener * listener, DWORD dwIoSize)
{
	char str[8192] = { 0 };
	for (size_t i = 0; i < dwIoSize; i++)
	{
		str[i] = listener->m_WsaBuf.buf[i];
	}

	if (0 == dwIoSize)
	{

	}
	else
	{
		char ip[64] = {0};
		inet_ntop(AF_INET, (void *)&(listener->m_PeerAddress.sin_addr.s_addr), ip, 64);
		//printf("%d Recv UDP %s:%d Data %s\n", listener->GetSocket(), ip, listener->m_PeerAddress.sin_port, str);

		int session_id = GetUDPSessionID(&listener->m_PeerAddress);
		if (0 == session_id)
		{
			session_id = NewSessionID();
			if (0 == session_id)
			{
				return false;
			}

			uint64_t key = listener->m_PeerAddress.sin_port; 
			key <<= 32; 
			key |= listener->m_PeerAddress.sin_addr.s_addr;
			SetUDPSessionID(&listener->m_PeerAddress, session_id);
			BaseSession *pSession = new UDPSession();
			pSession->Init(listener->GetSocket(), SOCK_UDP);
			pSession->SetSessionID(session_id);
			pSession->SetAddr(&listener->m_PeerAddress);
			pSession->SetIP(ip);
			pSession->SetPort(ntohs(listener->m_PeerAddress.sin_port));
			pSession->SetAlive(true);
			AddSession(pSession);

			m_pNetEventCallBack->OnAddConnection(pSession->GetSessionID(), pSession->GetSocket(), pSession->GetIP(), pSession->GetPort());
		}

		m_pNetEventCallBack->OnData(session_id, listener->GetSocket(), str, dwIoSize);
	}
	
	return true;
}


void NetService::SendWorker()
{

}

#endif // WIN32	
