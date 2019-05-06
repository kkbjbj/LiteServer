#pragma once
#include "unit_test_base.h"

#ifdef _WIN32 
#include <windows.h>
//#pragma comment (lib, "ws2_32.lib")
#else
#include <unistd.h>
#endif

class TestCallBack : public TimerCallBack
{
public:
	TestCallBack(int id, int cur, WheelTimer *timer) { mID = id; mCur = cur; mTimer = timer; }
	virtual ~TestCallBack() {}
	virtual void Execute() {
		++mCur;
		if (mCur - mID > 10)
		{
			return;
		}
		std::cout<<mID<<": "<<mCur<<std::endl;
		TestCallBack *call_back = new TestCallBack(mID, mCur, mTimer);
		mTimer->AddTimer(call_back, 100);
	}
private:
	int mCur;
	int mID;
	WheelTimer *mTimer;
};

class TimerThread : public ThreadBase
{
public:
	TimerThread(WheelTimer *pTimer) {
		m_pTimer = pTimer; mCount = 0;
	}
	~TimerThread() {}

	virtual void Run() {
		while (true)
		{
			m_pTimer->UpdateTime();
			if (++mCount > 6000)
			{
				break;
			}
#ifdef _WIN32 
			Sleep(1);
#else
			usleep(1 * 1000);
#endif

		};
	}
private:
	int mCount;
	WheelTimer *m_pTimer;
};


class TestTimer : public TestBase
{
public:
	TestTimer() {}
	~TestTimer() {}

	virtual bool Run()
	{
		WheelTimer timer;
		for (int i = 0; i < 1; ++i)
		{
			TestCallBack *call_back = new TestCallBack(i, i, &timer);
			timer.AddTimer(call_back, i);
		}

		TimerThread testThread(&timer);
		testThread.Run();

		testThread.Join();

		return true;
	}

private:

};

