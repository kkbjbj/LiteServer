#include "udp_listener.h"

UDPListener::UDPListener(string ip, int port)
{
	m_strIP = ip;
	m_nPort = port;
	m_Socket = SOCKET_ERROR;
	m_dwFlags = 0;
	m_dwBytes = 0;
	m_nFromlen = sizeof(sockaddr);
}

UDPListener::~UDPListener()
{

}

int UDPListener::CloseSocket()
{
	return DestroySocket(m_Socket);
}

bool UDPListener::Init()
{
	if (!m_Sock.Create(AF_INET, SOCK_DGRAM)) return false;
	m_Sock.SetReUseAddr();
	m_Sock.SetNonBlock();
	m_Socket = m_Sock.GetSocket();
	if (!m_Sock.Bind(m_strIP, m_nPort))
	{
		CloseSocket();
		return false;
	}

	m_IOContextReadFrom.Param = (void *)this;
	m_IOContextReadFrom.OperType = IO_READFROM;
	m_IOContextReadFrom.Socket  = m_Socket;
	m_IOContextReadFrom.olOverlap.Pointer = (PVOID)&m_IOContextReadFrom;

	m_WsaBuf.buf = m_Data;
	m_WsaBuf.len = 8192;

	m_pCompletionEvent.Socket = m_Socket;
	m_pCompletionEvent.SockType = SOCK_UDP;
	m_pCompletionEvent.Param = (void *)this;

	return true;
}



