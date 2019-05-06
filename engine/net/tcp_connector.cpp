#include "tcp_connector.h"

TCPConnector::TCPConnector(string ip, int port)
{
	m_strIP = ip;
	m_nPort = port;
	m_Socket = SOCKET_ERROR;
	m_Connected = false;
	m_lpfnConnectEx = NULL;
}

TCPConnector::~TCPConnector()
{
}

bool TCPConnector::Init()
{
	if (!m_TcpSession.GetSock()->Create(AF_INET, SOCK_STREAM)) return false;
	m_TcpSession.GetSock()->SetReUseAddr();
	m_TcpSession.GetSock()->SetNonBlock();
	m_Socket = m_TcpSession.GetSock()->GetSocket();
	m_TcpSession.GetSock()->Bind("0.0.0.0", 0);

	m_IOContextConnect.Param = (void *)this;
	m_IOContextConnect.OperType = IO_CONNECT;
	m_IOContextConnect.Socket = m_Socket;
	m_IOContextConnect.olOverlap.Pointer = (PVOID)&m_IOContextConnect;

	m_pCompletionEvent.Socket = m_Socket;
	m_pCompletionEvent.SockType = SOCK_CONNECT;
	m_pCompletionEvent.Param = (void *)this;

	memset(&m_ServerAddress,0, sizeof(sockaddr_in));
	m_ServerAddress.sin_family = AF_INET;
	m_ServerAddress.sin_port = htons(m_nPort);
	inet_pton(AF_INET, m_strIP.c_str(), &m_ServerAddress.sin_addr.s_addr);//addrPeer.sin_addr.s_addr = inet_addr("127.0.0.1");
	m_TcpSession.Init(m_Socket, SOCK_CONNECT);

	return GetFuncAddr();
}

bool TCPConnector::GetFuncAddr()
{
#ifdef WIN32
	DWORD dwBytes = 0;
	GUID GuidConnectEx = WSAID_CONNECTEX;
	if (SOCKET_ERROR == WSAIoctl(m_Socket, SIO_GET_EXTENSION_FUNCTION_POINTER,
		&GuidConnectEx, sizeof(GuidConnectEx),
		&m_lpfnConnectEx, sizeof(m_lpfnConnectEx), &dwBytes, 0, 0))
	{
		printf("WSAIoctl failed with error: %u\n", GetSockError());
		return false;
	}

#endif
	return true;
}

int TCPConnector::CloseSocket()
{
	return DestroySocket(m_Socket);
}