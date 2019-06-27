#include "unit_test_base.h"
#include "test_timer.h"
#include "test_buffer.h"
#include "test_object_pool.h"
#include "test_sock.h"
#include "test_lua.h"

using namespace std;


#ifdef _WIN32 
#include <windows.h>
#pragma comment (lib, "ws2_32.lib")
#else
#include <unistd.h>
#endif

int main(int argc, char *argv[])
{
	TestLua testlua;
	testlua.Run();

	TestSock testsock;
	testsock.Run();

	TestObjectPool testobjectpool;
	testobjectpool.Run();

	TestTimer testtimer;
	testtimer.Run();
	
	{
		AutoCpuCostTimer t;
		TestBuff testbuff;
		testbuff.Run();

	}
		
#ifdef _WIN32
	int temp;
	std::cin >> temp;
#endif
	return 0;
}