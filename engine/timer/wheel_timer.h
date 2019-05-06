#ifndef WHEEL_TIMER_H
#define WHEEL_TIMER_H

#include "util/type_def.h"
#include "thread/auto_lock.h"

#define TIME_NEAR_SHIFT 8
#define TIME_NEAR (1 << TIME_NEAR_SHIFT)
#define TIME_LEVEL_SHIFT 6
#define TIME_LEVEL (1 << TIME_LEVEL_SHIFT)
#define TIME_NEAR_MASK (TIME_NEAR-1)
#define TIME_LEVEL_MASK (TIME_LEVEL-1)

class TimerCallBack;
struct TimerNode
{
	uint32 expire;
	struct TimerNode * next;
	TimerCallBack *call_back;
};

struct TimerLinkList
{
	struct TimerNode head;
	struct TimerNode *tail;
};

class WheelTimer
{

public:
	WheelTimer();
	~WheelTimer();

public:
	void UpdateTime();
	void Update();
	void AddTimer(TimerCallBack * call_back, int time);

private:
	uint64 GetTime();
	struct TimerNode * ClearLink(struct TimerLinkList *list);
	void Link(struct TimerLinkList *list, struct TimerNode *node);
	void TimerExecute();
	void TimerShift();
	void MoveList(int level, int index);
	void AddNode(struct TimerNode *node);
	void DispatchList(struct TimerNode *list);
private:
	struct TimerLinkList mNear[TIME_NEAR];
	struct TimerLinkList mWheel[4][TIME_NEAR];
	AtomSpinLock m_Lock;
	uint32 m_uTime;
	uint64 m_uCurrentPoint;
};


#endif
