
#include "timer_callback.h"

TimerCallBack::TimerCallBack()
{
	m_bCancel = false;
}

TimerCallBack::~TimerCallBack()
{

}

void TimerCallBack::OnTimeOut()
{
	if (!m_bCancel)
	{
		Execute();
	}
}

void TimerCallBack::Cancel()
{

}

