#include <stdlib.h>
#include <string.h>

#include <chrono>
#include <iostream>

#include "wheel_timer.h"
#include "timer_callback.h"


WheelTimer::WheelTimer()
{
	for (int i = 0; i < TIME_NEAR; ++i)
	{
		ClearLink(&mNear[i]);
	}

	for (int i = 0; i < 4; ++i)
	{
		for (int j = 0; j < TIME_LEVEL; ++j)
		{
			ClearLink(&mWheel[i][j]);
		}
	}

	m_uCurrentPoint = GetTime();
}

WheelTimer::~WheelTimer()
{

}

//#ifdef _WIN32
//#include <winsock2.h>
//#include <time.h>
//#else
//#include <sys/time.h>
//#endif
//uint64 WheelTimer::GetTime()
//{
//	uint64 t;
//	struct timeval tv;
//#ifdef _WIN32 
//	
//	time_t clock;
//	struct tm tm;
//	SYSTEMTIME wtm;
//	GetLocalTime(&wtm);
//	tm.tm_year = wtm.wYear - 1900;
//	tm.tm_mon = wtm.wMonth - 1;
//	tm.tm_mday = wtm.wDay;
//	tm.tm_hour = wtm.wHour;
//	tm.tm_min = wtm.wMinute;
//	tm.tm_sec = wtm.wSecond;
//	tm.tm_isdst = -1;
//	clock = mktime(&tm);
//	tv.tv_sec = (long)clock;
//	tv.tv_usec = wtm.wMilliseconds * 1000;
//	t = (uint64)tv.tv_sec * 100;
//	t += tv.tv_usec / 10000;
//#else
//	gettimeofday(&tv, NULL);
//	t = (uint64)tv.tv_sec * 100;
//	t += tv.tv_usec / 10000;
//#endif
//	return t;
//}

uint64 WheelTimer::GetTime()
{
	uint64 t = std::chrono::time_point_cast<std::chrono::duration<long long, std::centi>>(std::chrono::system_clock::now()).time_since_epoch().count();
	return t;
}


struct TimerNode * WheelTimer::ClearLink(struct TimerLinkList *list)
{
	struct TimerNode *ret = list->head.next;
	list->head.next = NULL;
	list->tail = &(list->head);

	return ret;
}

void WheelTimer::Link(struct TimerLinkList *list, struct TimerNode *node)
{
	list->tail->next = node;
	list->tail = node;
	node->next = NULL;
}

void WheelTimer::Update()
{
	AutoLock<AtomSpinLock> lock(&m_Lock);
	TimerExecute();
	TimerShift();
	TimerExecute();
}

void WheelTimer::UpdateTime()
{
	uint64 cur_time = GetTime();
	if (cur_time < m_uCurrentPoint)
	{
		m_uCurrentPoint = cur_time;
	}
	else if(cur_time != m_uCurrentPoint)
	{
		int diff = (int)(cur_time - m_uCurrentPoint);
		m_uCurrentPoint = cur_time;
		for (int i = 0; i < diff; ++i)
		{
			Update();
		}
	}
}

void WheelTimer::TimerExecute()
{
	int index = m_uTime & TIME_NEAR_MASK;
	while(mNear[index].head.next)
	{
		struct TimerNode *node = ClearLink(&mNear[index]);
		m_Lock.Unlock();
		DispatchList(node);
		m_Lock.Lock();
	}
}

void WheelTimer::DispatchList(struct TimerNode *list)
{
	do
	{
		list->call_back->OnTimeOut();
		delete list->call_back; //Here?
		struct TimerNode *node = list;
		list = list->next;
		free(node);
	}while(list);
}

void WheelTimer::TimerShift()
{
	int mask = TIME_NEAR;
	uint32 cur_time = ++m_uTime;
	if(0 == cur_time)
	{
		MoveList(3, 0);
	}
	else
	{
		uint32 time = cur_time >> TIME_NEAR_SHIFT;
		int level = 0;
		while((cur_time & (mask - 1)) == 0)
		{
			int index = time & TIME_LEVEL_MASK;
			if (index != 0)
			{
				MoveList(level, index);
			}

			mask <<= TIME_LEVEL_SHIFT;
			time >>= TIME_LEVEL_SHIFT;
			++level;
		}
	}
}

void WheelTimer::MoveList(int level, int index)
{
	struct TimerNode *list = ClearLink(&mWheel[level][index]);
	while(list)
	{
		struct TimerNode *temp = list->next;
		AddNode(list);
		list = temp;
	}
}

void WheelTimer::AddNode(struct TimerNode *node)
{
	uint32 time = node->expire;
	uint32 cur_time = m_uTime;

	if ((time | TIME_NEAR_MASK) == (cur_time | TIME_NEAR_MASK))
	{
		Link(&mNear[time & TIME_NEAR_MASK], node);
	}
	else
	{
		int level = 0;
		uint32 mask = TIME_NEAR << TIME_LEVEL_SHIFT;
		for (level = 0; level < 3; ++level)
		{
			if ((time | (mask - 1)) == (cur_time | (mask - 1)))
			{
				break;
			}
		}

		Link(&mWheel[level][(time >> (TIME_NEAR_SHIFT + level * TIME_LEVEL_SHIFT)) & TIME_LEVEL_MASK], node);
	}
}

void WheelTimer::AddTimer(TimerCallBack * call_back, int time)
{
	struct TimerNode *node = (struct TimerNode *)malloc(sizeof(*node));
	node->call_back = call_back;
	node->next = NULL;
	AutoLock<AtomSpinLock> lock(&m_Lock);
	node->expire = time + m_uTime;
	AddNode(node);
}

