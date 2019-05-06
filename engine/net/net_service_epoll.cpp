#include <cstdio>
#include "net_service.h"
#include "tcp_session.h"
#include "tcp_connector.h"
#include "tcp_listener.h"
#include "udp_listener.h"
#include "udp_session.h"

#ifndef WIN32


bool NetService::Init(NetEventCallBack *pCallBack)
{
	InitSession();
	m_pNetEventCallBack = pCallBack;
	return true;
}

bool NetService::Start()
{
	if (NULL == m_pNetEventCallBack)
	{
		printf("m_pNetEventCallBack is null Call SetNetEventCallBack first.\n");
		return false;
	}

	m_ServiceHandle = epoll_create(1);
	if (m_ServiceHandle <= 0)
	{
		printf("CreateEpoll error %s\n",strerror(errno));
		return false;
	}

	m_bRunning = true;

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
	return true;
}

bool NetService::CleanUp()
{
	return true;
}

void NetService::ThreadProc()
{
	while (m_bRunning) 
	{
		m_pNetEventCallBack->OnUpdate();
		int event_num = epoll_wait(m_ServiceHandle, m_Events, MAX_SOCK_EVENT, 1);
		if (event_num <= 0)
		{
			continue;
		}

		for (int i = 0; i < event_num; ++i)
		{
			SOCK_IO_CONTEXT *io_context = (SOCK_IO_CONTEXT *)m_Events[i].data.ptr;
			SOCKET sock = io_context->Socket;
			
			switch(io_context->OperType)
			{
				
				case IO_ACCEPT:
				{
					if( m_Events[i].events & EPOLLIN )
					{
						TCPListener * listener = (TCPListener *)io_context->Param;
						PostAccept(listener);
					}
					
				}
				break;
				case IO_CONNECT:
				{
					if( m_Events[i].events & EPOLLOUT )
					{
						TCPConnector *pTCPConnector = (TCPConnector *)io_context->Param;
						PostConnect(pTCPConnector);
					}
					
				}
				break;
				case IO_READ:
				{
					if( m_Events[i].events & EPOLLIN )
					{
						TCPSession *pTcpSession = (TCPSession *)io_context->Param;
						PostRead(pTcpSession);
					}
				}
				break;
				case IO_READFROM:
				{
					if( m_Events[i].events & EPOLLIN )
					{
						UDPListener * listener = (UDPListener *)io_context->Param;
						PostReadFrom(listener);
					}
				}
				break;
				default:
				break;
			}
			
		}
	}

}

bool NetService::AddTCPListener(TCPListener * listener)
{
	SOCK_IO_CONTEXT *io_context = listener->NewIOContext(0);
	io_context->Socket = listener->GetSocket();
	io_context->OperType = IO_ACCEPT;
	io_context->Param = (void *)listener;

	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = io_context;
	if (epoll_ctl(m_ServiceHandle, EPOLL_CTL_ADD, io_context->Socket, &ev) < 0) 
	{
		printf("epoll add listen error: %s\n",strerror(errno));
		return false;
	}

	return true;
}

bool NetService::AddUDPListener(UDPListener * listener)
{
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = (void *)&listener->m_IOContextReadFrom;
	if (epoll_ctl(m_ServiceHandle, EPOLL_CTL_ADD, listener->GetSocket(), &ev) < 0) 
	{
		printf("epoll add UDPListener error: %s\n",strerror(errno));
		return false;
	}

	return true;
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

	struct epoll_event ev;
	ev.events = EPOLLOUT | EPOLLET;
	ev.data.ptr = (void *)&pConnector->m_IOContextConnect;
	if (epoll_ctl(m_ServiceHandle, EPOLL_CTL_ADD, pConnector->GetSocket(), &ev) < 0) 
	{
		FreeSessionID(session_id);
		printf("epoll add connector error: %s\n",strerror(errno));
		return false;
	}

	int ret = connect(pConnector->GetSocket(), (struct sockaddr*)&pConnector->m_ServerAddress, sizeof(sockaddr_in));
	if(ret < 0) 
	{
	    if( errno != EINPROGRESS ) 
	    {
	    	FreeSessionID(session_id);
	        printf("connect fail :%s \n", strerror(errno));
	        return false;
	    }
	}

	return true;
}

bool NetService::PostAccept(TCPListener * listener)
{
	SOCKET accept_sock = 0;
	struct sockaddr_in client_ddr;
	memset(&client_ddr, 0, sizeof(sockaddr_in));
	socklen_t socklen = sizeof(struct sockaddr_in);
	while(true)
	{
		accept_sock = accept(listener->GetSocket(), (struct sockaddr *)&client_ddr, &socklen);
		if(accept_sock == -1)
		{
			break;
		}


	
		char ip[128] = { 0 };
		const char * pip = NULL;
		inet_ntop(AF_INET, (void *)&client_ddr.sin_addr.s_addr, ip, socklen);

		printf("Accept %s:%d \n", ip, ntohs(client_ddr.sin_port));

		int session_id = NewSessionID();
		if (0 == session_id)
		{
			DestroySocket(accept_sock);
			return false;
		}

		TCPSession *pTcpSession = new TCPSession();
		pTcpSession->SetSessionID(session_id);
		pTcpSession->Init(accept_sock, SOCK_ACCEPTED);
		pTcpSession->SetAddr(&client_ddr);
		pTcpSession->SetIP(ip);
		pTcpSession->SetPort(ntohs(client_ddr.sin_port));
		pTcpSession->SetAlive(true);

		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		ev.data.ptr = (void *)&pTcpSession->m_IOContextRead;
		if (epoll_ctl(m_ServiceHandle, EPOLL_CTL_ADD, accept_sock, &ev) < 0) 
		{
			delete pTcpSession;
			FreeSessionID(session_id);
			DestroySocket(accept_sock);
			printf("epoll add accepted error: %s\n",strerror(errno));
			break;
		}

		AddSession(pTcpSession);
		m_pNetEventCallBack->OnAddConnection(pTcpSession->GetSessionID(), pTcpSession->GetSocket(), pTcpSession->GetIP(), pTcpSession->GetPort());
	}

	return true;
}

bool NetService::DoAccpet(AcceptBuff * buff, DWORD dwIoSize)
{
	return true;
}

bool NetService::PostConnect(TCPConnector *pTCPConnector)
{
	pTCPConnector->SetConnected();
	AddSession(&pTCPConnector->m_TcpSession);
	m_pNetEventCallBack->OnConnected(pTCPConnector->m_TcpSession.GetSessionID(), pTCPConnector->m_TcpSession.GetSocket());
	
	struct epoll_event ev;
	ev.events = EPOLLIN | EPOLLET;
	ev.data.ptr = (void *)&pTCPConnector->m_TcpSession.m_IOContextRead;
	if (epoll_ctl(m_ServiceHandle, EPOLL_CTL_MOD, pTCPConnector->GetSocket(), &ev) < 0) 
	{
		pTCPConnector->CloseSocket();
		OnSessionDrop(pTCPConnector->GetSession());
		printf("epoll add connector error: %s\n",strerror(errno));
		return false;
	}

	pTCPConnector->GetSession()->SetAlive(true);

	return true;
}

bool NetService::DoConnect(TCPConnector *pTCPConnector, DWORD dwIoSize)
{
	return true;
}

bool NetService::PostRead(TCPSession *pTcpSession)
{
	int nread = 0;
	int size = 0;
	char data[1024] = { 0 };
	while ((nread = read(pTcpSession->GetSocket(), data, 1024)) > 0)
	{
		//printf("%d recv %s\n", pTcpSession->GetSocket(), data);
		size = size + nread; 
	}

	if (nread == -1 && errno != EAGAIN)
	{
		struct epoll_event ev;
		ev.events = EPOLLIN | EPOLLET;
		if (epoll_ctl(m_ServiceHandle, EPOLL_CTL_DEL, pTcpSession->GetSocket(), &ev) < 0) 
		{
			printf("epoll del error: %s\n",strerror(errno));
		}

		printf("%s peer disconnect sock=%d\n", strerror(errno), pTcpSession->GetSocket());
		OnSessionDrop(pTcpSession);
		return false;
	}

	m_pNetEventCallBack->OnData(pTcpSession->GetSessionID(), pTcpSession->GetSocket(), data, size);

	return true;
}

bool NetService::DoRead(TCPSession *pTcpSession, DWORD dwIoSize)
{
	return true;
}

bool NetService::PostReadFrom(UDPListener * listener)
{
	int nread = 0;
	char data[1024] = { 0 };
	socklen_t fromlen = sizeof(sockaddr);

	while ((nread = recvfrom(listener->GetSocket(), data, 1024, 0, (struct sockaddr*)&listener->m_PeerAddress, &fromlen)) > 0)
	{
		int session_id = GetUDPSessionID(&listener->m_PeerAddress);
		if (0 == session_id)
		{
			session_id = NewSessionID();
			if (0 == session_id)
			{
				return false;
			}

			char ip[128] = { 0 };
			inet_ntop(AF_INET, (void *)&listener->m_PeerAddress.sin_addr.s_addr, ip, sizeof(struct sockaddr_in));

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
		m_pNetEventCallBack->OnData(session_id, listener->GetSocket(), data, nread);

		//printf("%d recvfrom  %s\n", listener->GetSocket(), data);
	}

	if (nread == -1 && errno != EAGAIN)
	{
		printf("%s recvfrom peer error sock=%d\n", strerror(errno), listener->GetSocket());
		return false;
	}

	return true;
}

bool NetService::DoReadFrom(UDPListener * listener, DWORD dwIoSize)
{
	return true;
}


void NetService::SendWorker()
{
	while (m_bRunning) 
	{
	}
}

#endif // WIN32	
