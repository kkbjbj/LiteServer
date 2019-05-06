#ifndef CPU_COST_TIMER_H
#define CPU_COST_TIMER_H

#include "type_def.h"

struct CpuTimes
{
	uint64 twall;
	uint64 tuser;
	uint64 tsystem;
	CpuTimes():twall(0),tuser(0),tsystem(0){}
	void Clear() { twall = tuser = tsystem = 0; }
};

class CpuCostTimer
{
public:
	CpuCostTimer() {}
	~CpuCostTimer() {}
	void Start();
	void Stop();
	bool IsStoped();
	void Resume();
	CpuTimes Elapsed();
	void GetCpuTimes(CpuTimes &times);

protected:
	CpuTimes m_Times;
private:
	bool m_bStop;
};


class AutoCpuCostTimer : public CpuCostTimer
{
public:
	AutoCpuCostTimer();
	~AutoCpuCostTimer();
private:
	void Show();
};

#endif //CPU_COST_TIMER_H



