#include "net_service.h"
#include "tcp_listener.h"
#include "udp_listener.h"
#include "tcp_connector.h"
#include "tcp_session.h"
#include "udp_session.h"

NetService::NetService()
{
	m_bRunning = false;
	m_pNetEventCallBack = NULL;
}

NetService::~NetService()
{
	
}

int NetService::NewSessionID()
{
	if (!m_queSessionID.empty())
	{
		int id = m_queSessionID.front();
		m_queSessionID.pop();
		return id;
	}

	return 0;
}

void NetService::FreeSessionID(int SessionID)
{
	m_queSessionID.push(SessionID);
}

void NetService::InitSession()
{
	std::queue<int> temp;
	m_queSessionID.swap(temp);
	for (size_t i = 0; i < MAX_SOCK_EVENT; i++)
	{
		m_Sessions[i] = NULL;
		m_queSessionID.push(i + 1);
	}
}

void NetService::AddSession(BaseSession *pSession)
{
	m_Sessions[pSession->GetSessionID()] = pSession;
	//uint64_t key = pSession->GetSessionKey();
	//m_mapSessions[key] = pSession;
}

void NetService::DelSession(BaseSession *pSession)
{
	int id = pSession->GetSessionID();
	if (NULL != m_Sessions[id])
	{
		m_Sessions[id]->SetAlive(false);
		if (m_Sessions[id]->GetSockType() != SOCK_CONNECT)
		{
			if (m_Sessions[id]->GetSockType() == SOCK_ACCEPTED)
			{
				m_Sessions[id]->GetSock()->Close();
			}
			if (m_Sessions[id]->GetSockType() == SOCK_UDP)
			{
				SetUDPSessionID(m_Sessions[id]->GetAddr(), 0);
			}
			
			delete m_Sessions[id];
			m_Sessions[id] = NULL;
			FreeSessionID(id);
		}
	}
	m_Sessions[id] = NULL;
	//m_mapSessions.erase(pSession->GetSessionKey());
}

bool NetService::KickSession(int SessionID, SOCKET socket)
{
	printf("Kick %d\n", SessionID);
	if (NULL != m_Sessions[SessionID])
	{
		if (m_Sessions[SessionID]->GetSockType() != SOCK_UDP)
		{
			m_Sessions[SessionID]->GetSock()->Close();
		}
		
		DelSession(m_Sessions[SessionID]);
	}

	return true;
}

bool NetService::OnSessionDrop(BaseSession *pSession)
{
	printf("session Drop %d\n", pSession->GetSessionID());
	bool ret = false;
	if (SOCK_CONNECT == pSession->GetSockType())
	{
		ret = m_pNetEventCallBack->OnConnectFail(pSession->GetSessionID(), pSession->GetSocket());
	}
	else
	{
		ret = m_pNetEventCallBack->OnConnectionDrop(pSession->GetSessionID(), pSession->GetSocket());
		DelSession(pSession);
	}

	return true;
}

int NetService::GetUDPSessionID(struct sockaddr_in *pAddress)
{
	uint64_t key = pAddress->sin_port; 
	key <<= 32; 
	key |= pAddress->sin_addr.s_addr;
	std::map<uint64_t, int>::iterator it = m_mapUDPSessions.find(key);
	if (it == m_mapUDPSessions.end())
	{
		return 0;
	}
	else
	{
		return it->second;
	}
}

void NetService::SetUDPSessionID(struct sockaddr_in *pAddress, int SessionID)
{
	uint64_t key = pAddress->sin_port;
	key <<= 32;
	key |= pAddress->sin_addr.s_addr;
	if (SessionID == 0)
	{
		m_mapUDPSessions.erase(key);
	}
	else
	{
		m_mapUDPSessions[key] = SessionID;
	}
}