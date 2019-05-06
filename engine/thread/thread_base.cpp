#include "thread_base.h"

ThreadBase::ThreadBase()
	:m_bRunning(false)
	,m_pThread(NULL)
{

}

ThreadBase::~ThreadBase()
{

}

void ThreadBase::ThreadProc()
{
	Run();
}

bool  ThreadBase::Start()
{
	if (m_bRunning)
	{
		return false;
	}

	
	m_bRunning = true;

	m_pThread = new std::thread(std::bind(&ThreadBase::ThreadProc, this));

	return true;
	
}

bool ThreadBase::Join()
{
	if (!m_bRunning)
	{
		return false;
	}

	m_pThread->join();
	return true;
}

bool ThreadBase::IsRunning()
{
	return m_bRunning;
}