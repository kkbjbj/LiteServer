#ifndef THREAD_BASE_H
#define THREAD_BASE_H

#include <thread>

class ThreadBase
{
public:
	ThreadBase();
	virtual ~ThreadBase();

	void ThreadProc();

	bool Start();
	bool Join();
	bool IsRunning();

	virtual void Run() = 0;

private:
	bool m_bRunning;
	std::thread *m_pThread;
};
     
#endif
