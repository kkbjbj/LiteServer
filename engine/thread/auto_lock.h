#ifndef AUTO_LOCK_H
#define AUTO_LOCK_H

#include "spin_lock.h"
#include "atom_spin_lock.h"

template<class Lock>
class AutoLock
{
public:
	AutoLock(Lock *lock)
		:m_pLock(lock)
	{
		m_pLock->Lock();
	}
	~AutoLock()
	{
		m_pLock->Unlock();
	}

	AutoLock(const AutoLock&) = delete;
	AutoLock& operator= (const AutoLock&) = delete;
private:
	Lock *m_pLock;
};

#endif