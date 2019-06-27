#pragma once
#include "unit_test_base.h"

class TestNetCallBack : public NetEventCallBack
{
public:
	virtual bool OnAddConnection(int SessionID, SOCKET Sock, std::string IP, int Port)
	{
		printf("OnAddConnection Session:%d Sock:%d IP:%s, Port:%d\n", SessionID, (int)Sock, IP.c_str(), Port);
		return true;
	}
	virtual bool OnConnectionDrop(int SessionID, SOCKET Sock)
	{
		printf("OnConnectionDrop Session:%d Sock:%d \n", SessionID, (int)Sock);
		return true;
	}
	virtual bool OnConnected(int SessionID, SOCKET Sock)
	{
		printf("OnConnected Session:%d Sock:%d \n", SessionID, (int)Sock);
		return true;
	}
	virtual bool OnConnectFail(int SessionID, SOCKET Sock)
	{
		printf("OnConnectFail Session:%d Sock:%d \n", SessionID, (int)Sock);
		return true;
	}
	virtual bool OnData(int SessionID, SOCKET Sock, char *Data, int size)
	{
		printf("OnData Session:%d Sock:%d size:%d %s\n", SessionID, (int)Sock, size, Data);
		return true;
	}
	virtual bool OnUpdate() { return true; }
};


class TestSock : public TestBase
{
public:
	TestSock() {}
	~TestSock() {}

	virtual bool Run()
	{
		TestNetCallBack callback;
		NetService *service = new NetService;
		service->Init(&callback);
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
			else
			{			
				listener2.GetSock()->SendTo(c.c_str(), (int)c.length() + 1, "127.0.0.1", 8091);
				connector.GetSock()->Send(c.c_str(), (int)c.length() + 1);
				connector2.GetSock()->Send(c.c_str(), (int)c.length() + 1);
			}

		}
			
		service->Stop();
		service->CleanUp();

		std::cout<<"Bye\n";
		return true;
	}

private:

};