
#ifdef WIN32
	#pragma comment(lib, "ws2_32.lib")
	#pragma comment(lib, "Mswsock.lib")
#endif

#include <iostream>

#include "net_service.h"
#include "tcp_listener.h"
#include "udp_listener.h"
#include "tcp_connector.h"
#include "sock.h"
#include "sock_struct.h"

class TestCallBack : public NetEventCallBack
{
public:
	virtual bool OnAddConnection(int SessionID, SOCKET Sock, std::string IP, int Port)
	{ 
		printf("OnAddConnection Session:%d Sock:%d IP:%s, Port:%d\n", SessionID, Sock, IP.c_str(), Port);  
		return true;
	}
	virtual bool OnConnectionDrop(int SessionID, SOCKET Sock)
	{ 
		printf("OnConnectionDrop Session:%d Sock:%d \n", SessionID, Sock);  
		return true;
	}
	virtual bool OnConnected(int SessionID, SOCKET Sock)
	{ 
		printf("OnConnected Session:%d Sock:%d \n", SessionID, Sock);  
		return true;
	}
	virtual bool OnConnectFail(int SessionID, SOCKET Sock)
	{ 
		printf("OnConnectFail Session:%d Sock:%d \n", SessionID, Sock);  
		return true;
	}
	virtual bool OnData(int SessionID, SOCKET Sock, char *Data, int size)
	{ 
		printf("OnData Session:%d Sock:%d size:%d %s\n", SessionID, Sock, size, Data);  
		return true; 
	}
	virtual bool OnUpdate() {  return true; }
};

int main()
{
	TestCallBack *pcallback = new TestCallBack;
	NetService *service = new NetService;
	service->Init(pcallback);
	service->Start();

	TCPListener listener("0.0.0.0", 8080);
	listener.Init();
	service->AddTCPListener(&listener);

	UDPListener listener2("0.0.0.0", 8090);
	listener2.Init();
	service->AddUDPListener(&listener2);

	UDPListener listener3("0.0.0.0", 8091);
	listener3.Init();
	service->AddUDPListener(&listener3);

	TCPConnector connector("127.0.0.1", 8080);
	connector.Init();
	service->AddTCPConnector(&connector);

	TCPConnector connector2("127.0.0.1", 8080);
	connector2.Init();
	service->AddTCPConnector(&connector2);

	string c = "data test";
	std::cin >> c;
	while (true)
	{
		std::cin >> c;
		if (c == "Q")
		{
			break;
		}
		else if (c == "c")
		{
			service->KickSession(5, 0);
			service->KickSession(4, 0);
			service->KickSession(3, 0);
			service->KickSession(2, 0);
			service->KickSession(1, 0);
		}
		else
		{			
			listener2.GetSock()->SendTo(c.c_str(), c.length() + 1, "127.0.0.1", 8091);
			connector.GetSock()->Send(c.c_str(), c.length() + 1);
			connector2.GetSock()->Send(c.c_str(), c.length() + 1);
		}

	}
		
	service->Stop();
	service->CleanUp();

	std::cout<<"Bye\n";

	return 0;

	return 0;
}