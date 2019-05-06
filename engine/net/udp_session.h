#include "base_session.h"

class UDPSession : public BaseSession
{
public:
	UDPSession() {}
	~UDPSession() {}

	virtual void Init(SOCKET Socket, SOCK_TYPE SockType)
	{
		m_Socket = Socket;
		m_nSockType = SockType;
		m_Sock.SetSocket(Socket);
	}
	virtual bool Send(const char* Data, unsigned int Len) { return true; }
	virtual bool DoSend() { return true; }
private:
	
};
