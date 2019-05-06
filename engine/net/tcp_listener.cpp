#include "tcp_listener.h"

TCPListener::TCPListener(string ip, int port)
{
	m_strIP = ip;
	m_nPort = port;
	m_Socket = SOCKET_ERROR;
	m_pfnAcceptEx = NULL;
	m_pfnAcceptExSockAddr = NULL;
}

TCPListener::~TCPListener()
{

}

bool TCPListener::Init()
{
	if (!m_Sock.Create(AF_INET, SOCK_STREAM)) return false;
	m_Sock.SetReUseAddr();
	m_Sock.SetNonBlock();
	m_Socket = m_Sock.GetSocket();
	if (!m_Sock.Bind(m_strIP, m_nPort))
	{
		CloseSocket();
		return false;
	}

	if (!m_Sock.Listen())
	{
		CloseSocket();
		return false;
	}

	m_pCompletionEvent.Socket = m_Socket;
	m_pCompletionEvent.SockType = SOCK_LISTEN;
	m_pCompletionEvent.Param = (void *)this;

	return GetFuncAddr();
}

bool TCPListener::GetFuncAddr()
{
#ifdef WIN32
	GUID GuidAcceptEx = WSAID_ACCEPTEX;
	GUID GuidGetAcceptExSockAddrs = WSAID_GETACCEPTEXSOCKADDRS;

	DWORD dwBytes = 0;
	if (SOCKET_ERROR == WSAIoctl(m_Socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidAcceptEx, sizeof(GuidAcceptEx),
		&m_pfnAcceptEx, sizeof(m_pfnAcceptEx),
		&dwBytes, NULL, NULL))
	{
		printf("WSAIoctl failed with error: %u\n", GetSockError());
		CloseSocket();
		return false;
	}

	if (SOCKET_ERROR == WSAIoctl(m_Socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidGetAcceptExSockAddrs, sizeof(GuidGetAcceptExSockAddrs),
		&m_pfnAcceptExSockAddr, sizeof(m_pfnAcceptExSockAddr),
		&dwBytes, NULL, NULL))
	{
		printf("WSAIoctl failed with error: %u\n", GetSockError());
		CloseSocket();
		return false;
	}
#endif
	return true;
}

int TCPListener::CloseSocket()
{
	return DestroySocket(m_Socket);
}

SOCK_IO_CONTEXT *TCPListener::NewIOContext(SOCKET AcceptSocket)
{
	if (m_queIOContext.empty())
	{
		SOCK_IO_CONTEXT * pIOContext = new SOCK_IO_CONTEXT();
		AcceptBuff * pBuff = NewAcceptBuff(AcceptSocket);

		pIOContext->OperType = IO_ACCEPT;
		pIOContext->Param = pBuff;
		pIOContext->olOverlap.Pointer = (PVOID)pIOContext;
		return pIOContext;
	}

	SOCK_IO_CONTEXT * pIOContext = m_queIOContext.front();
	m_queIOContext.pop();

	AcceptBuff * pBuff = (AcceptBuff *)pIOContext->Param;
	pBuff->AcceptSocket = AcceptSocket;
	return pIOContext;
}
void TCPListener::DelIOContext(SOCK_IO_CONTEXT *pIOContext)
{
	m_queIOContext.push(pIOContext);
}

AcceptBuff * TCPListener::NewAcceptBuff(SOCKET AcceptSocket)
{
	if (m_queAcceptBuff.empty())
	{
		AcceptBuff * pBuff =  new AcceptBuff();
		pBuff->AcceptSocket = AcceptSocket;
		pBuff->pListener = this;
		return pBuff;
	}

	AcceptBuff * buff = m_queAcceptBuff.front();
	m_queAcceptBuff.pop();
	buff->AcceptSocket = AcceptSocket;
	return buff;
}

void TCPListener::DelAcceptBuff(AcceptBuff *buff)
{
	m_queAcceptBuff.push(buff);
}