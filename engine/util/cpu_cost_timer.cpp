#include <iostream>
#include "cpu_cost_timer.h"
#include <chrono>

#ifdef _WIN32
   #include <windows.h>
#else
   #include <unistd.h>
   #include <sys/times.h>
#endif

void CpuCostTimer::Start()
{
	m_bStop = false;
	GetCpuTimes(m_Times);
}

void CpuCostTimer::Stop()
{
	if (IsStoped())
	{
		return;
	}
	m_bStop = true;
	CpuTimes cur;
	GetCpuTimes(cur);

	m_Times.twall = (cur.twall - m_Times.twall);
    m_Times.tuser = (cur.tuser - m_Times.tuser);
    m_Times.tsystem = (cur.tsystem - m_Times.tsystem);
}

bool CpuCostTimer::IsStoped()
{
	return m_bStop;
}

void CpuCostTimer::Resume()
{
	if (!IsStoped())
	{
		return;
	}

	CpuTimes cur(m_Times);
	Start();
	m_Times.twall 	-= cur.twall;
    m_Times.tuser 	-= cur.tuser;
    m_Times.tsystem -= cur.tsystem;
}

CpuTimes CpuCostTimer::Elapsed()
{
	if (IsStoped())
	{
		return m_Times;
	}

	CpuTimes cur;
	GetCpuTimes(cur);
	cur.twall -= m_Times.twall;
   	cur.tuser -= m_Times.tuser;
   	cur.tsystem -= m_Times.tsystem;
   	return cur;
}

void CpuCostTimer::GetCpuTimes(CpuTimes &times)
{
	std::chrono::duration<uint64, std::nano> x(std::chrono::high_resolution_clock::now().time_since_epoch());
	times.twall = x.count();
# if defined(_WIN32)

    FILETIME creation, exit;
    if (::GetProcessTimes(::GetCurrentProcess(), &creation, &exit,
            (LPFILETIME)&times.tsystem, (LPFILETIME)&times.tuser))
    {
      times.tuser   *= 100;
      times.tsystem *= 100;
    }
    else
    {
      times.tsystem = times.tuser = -1;
    }
# else
    tms tm;
    clock_t c = ::times(&tm);
	static uint64 tick_factor = 0;
	if (!tick_factor)
	{
		if ((tick_factor = ::sysconf(_SC_CLK_TCK)) <= 0)
			tick_factor = -1;
		else
		{
			tick_factor = uint64(1000000000) / tick_factor;  // compute factor
			if (!tick_factor)
				tick_factor = -1;
		}
	}

    if (c == static_cast<clock_t>(-1))
    {
      times.tsystem = times.tuser = -1;
    }
    else
    {
      times.tsystem = uint64(tm.tms_stime + tm.tms_cstime);
      times.tuser = uint64(tm.tms_utime + tm.tms_cutime);
	  times.tuser *= tick_factor;
	  times.tsystem *= tick_factor;
    }
# endif
}

AutoCpuCostTimer::AutoCpuCostTimer()
{
	Start();
}

AutoCpuCostTimer::~AutoCpuCostTimer()
{
	Stop();
	Show();
}

void AutoCpuCostTimer::Show()
{
	const double sec = 1000000000.0L;
	uint64 total = m_Times.tsystem + m_Times.tuser;
	double wall_sec = static_cast<double>(m_Times.twall) / sec;
	double total_sec = static_cast<double>(total) / sec;
	double user_sec = static_cast<double>(m_Times.tuser) / sec;
	double system_sec = static_cast<double>(m_Times.tsystem) / sec;
	int p = 0;
	if (wall_sec > 0.001L && total_sec > 0.001L)
	{
		p = int(total_sec / wall_sec * 100);
	}
	
	std::cout << "user: " <<user_sec<< "s system: "<< system_sec<< "s " << " cpu: " << p<<"%" << std::endl;
}





