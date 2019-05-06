#pragma once

#include <iostream>
#include "engine.h"

#ifdef _WIN32 
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif


void TestSleep(int t)
{
#ifdef _WIN32 
	Sleep(t);
#else
	usleep(t * 1000);
#endif
}

uint64 TestGetTime()
{
	uint64 t;
	struct timeval tv;
#ifdef _WIN32 

	time_t clock;
	struct tm tm;
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	tm.tm_year = wtm.wYear - 1900;
	tm.tm_mon = wtm.wMonth - 1;
	tm.tm_mday = wtm.wDay;
	tm.tm_hour = wtm.wHour;
	tm.tm_min = wtm.wMinute;
	tm.tm_sec = wtm.wSecond;
	tm.tm_isdst = -1;
	clock = mktime(&tm);
	tv.tv_sec = (long)clock;
	tv.tv_usec = wtm.wMilliseconds * 1000;
	t = (uint64)tv.tv_sec * 100;
	t += tv.tv_usec / 10000;
#else
	gettimeofday(&tv, NULL);
	t = (uint64)tv.tv_sec * 100;
	t += tv.tv_usec / 10000;
#endif
	return t;
}

class TestBase
{
public:
	TestBase() {}
	~TestBase() {}
	virtual bool Run() = 0;
private:
};