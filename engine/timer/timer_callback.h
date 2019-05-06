#ifndef TIMER_CALLBACK_H
#define TIMER_CALLBACK_H

class TimerCallBack
{
public:
	TimerCallBack();
	virtual ~TimerCallBack();
	void OnTimeOut();
	virtual void Execute() = 0;
	void Cancel();
private:
	bool m_bCancel;
};

#endif