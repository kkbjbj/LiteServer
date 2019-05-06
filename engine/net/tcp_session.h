#pragma once

#include <string>
#include "sock_struct.h"
#include "sock.h"
#include "base_session.h"

using std::string;

class TCPSession : public BaseSession
{
public:
	TCPSession() {}
	~TCPSession() {}

	void Init(SOCKET Socket, SOCK_TYPE SockType)
	{
		m_Socket = Socket;
		m_nSockType = SockType;
		m_Sock.SetSocket(Socket);
		m_Sock.SetNonBlock();
		m_Sock.SetReUseAddr();
		m_CompletionEvent.Socket = m_Socket;
		m_CompletionEvent.SockType = SockType;
		m_CompletionEvent.Param = (void *)this;

		m_IOContextRead.Param = (void *)this;
		m_IOContextRead.OperType = IO_READ;
		m_IOContextRead.Socket = m_Socket;
		m_IOContextRead.olOverlap.Pointer = (PVOID)&m_IOContextRead;
		m_WsaBuf.buf = m_Data;
		m_WsaBuf.len = 8192;
		m_dwFlags = 0;
		m_dwBytes = 0;
	}

	virtual bool Send(const char* Data, unsigned int Len) { return true; }
	virtual bool DoSend() { return true; }
private:
	
public:	
	SOCK_IO_CONTEXT m_IOContextRead;

	CompletionEvent m_CompletionEvent;
	WSABUF  m_WsaBuf;
	DWORD m_dwFlags;
	DWORD m_dwBytes;
};