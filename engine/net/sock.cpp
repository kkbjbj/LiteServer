#include "sock.h"

bool Sock::Create(int Family, int Type)
{
#ifdef WIN32
	m_Socket = WSASocket(Family, Type, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
	if (INVALID_SOCKET == m_Socket) return false;

#else
	m_Socket = socket(Family, Type, 0);
	if (INVALID_SOCKET == m_Socket) return false;	
#endif
	return true;
}

void Sock::SetSocket(SOCKET Socket)
{
	m_Socket = Socket;
}

bool Sock::SetNonBlock()
{
#ifndef WIN32
	if (fcntl(m_Socket, F_SETFL, fcntl(m_Socket, F_GETFL, 0) | O_NONBLOCK) < 0) return false;
#endif // WIN32
	return true;
}

bool Sock::SetReUseAddr()
{
	int opt = 1;
	int ret = setsockopt(m_Socket, SOL_SOCKET, SO_REUSEADDR, (const char *)&opt, sizeof(opt));
	if (ret < 0)
	{
		printf("setsockopt: %u\n", GetSockError());
		return false;
	}

	return true;
}

bool Sock::Bind(string IP, int Port)
{
	m_strIP = IP;
	memset((char *)&m_Address,0, sizeof(m_Address));
	m_Address.sin_family = AF_INET;
	inet_pton(AF_INET, IP.c_str(), &m_Address.sin_addr.s_addr); //ServerAddress.sin_addr.s_addr = htonl(INADDR_ANY); 
	m_Address.sin_port = htons(Port);
	if (SOCKET_ERROR == bind(m_Socket, (struct sockaddr *) &m_Address, sizeof(m_Address)))
	{
		printf("bind failed with error: %u\n", GetSockError());
		return false;
	}

	return true;
}

bool Sock::Listen(int MaxConn /*= 1024*/)
{
	if (SOCKET_ERROR == listen(m_Socket, MaxConn))
	{
		printf("listen failed with error: %u\n", errno);
		return false;
	}
	return true;
}

int Sock::Send(const char* Data, unsigned int Len)
{
	int write_count = 0;
	int left = Len;
	while (left > 0)
	{
		write_count = ::send(m_Socket, Data, left, 0);
		if (write_count < 0)
		{
			if (errno == EAGAIN)
			{
				continue;
			}
			else
			{
				printf("%d Send failed with error: %u\n", m_Socket, GetSockError());
				return -1;
			}
		}
		else if (write_count == left)
		{
			return 0;
		}
		else
		{
			left -= write_count;
			Data += write_count;
		}
	}
	return left;
}

int Sock::SendTo(const char* Data, unsigned int Len, struct sockaddr *PeerAddr)
{
	int write_count = 0;
	int left = Len;
	while (left > 0)
	{
		write_count = ::sendto(m_Socket, Data, left, 0, (struct sockaddr *) &PeerAddr, sizeof(struct sockaddr));
		if (write_count < 0)
		{
			if (errno == EAGAIN)
			{
				continue;
			}
			else
			{
				printf("%d SendTo failed with error: %u\n", m_Socket, GetSockError());
				return -1;
			}
		}
		else if (write_count == left)
		{
			return 0;
		}
		else
		{
			left -= write_count;
			Data += write_count;
		}
	}
	return left;
}

int Sock::SendTo(const char* Data, unsigned int Len, string IP, int Port)
{
	struct sockaddr_in PeerAddress;
	memset(&PeerAddress, 0, sizeof(sockaddr_in));
	PeerAddress.sin_family = AF_INET;
	PeerAddress.sin_port = htons(Port);
	inet_pton(AF_INET, IP.c_str(), &PeerAddress.sin_addr.s_addr);

	int write_count = 0;
	int left = Len;
	while (left > 0)
	{
		write_count = ::sendto(m_Socket, Data, left, 0, (struct sockaddr *) &PeerAddress, sizeof(struct sockaddr));
		if (write_count < 0)
		{
			if (errno == EAGAIN)
			{
				continue;
			}
			else
			{
				printf("%d SendTo failed with error: %u\n", m_Socket, GetSockError());
				return -1;
			}
		}
		else if (write_count == left)
		{
			return 0;
		}
		else
		{
			left -= write_count;
			Data += write_count;
		}
	}
	return left;
}


int Sock::Close()
{
	int ret = -1;
	if (INVALID_SOCKET != m_Socket)
	{
		ret = DestroySocket(m_Socket);
		m_Socket = INVALID_SOCKET;
	}
	return ret;
}
