#ifndef SPIN_LOCK_H
#define SPIN_LOCK_H

#include <mutex>

class SpinLock
{
public:
	SpinLock() {}
	~SpinLock() {}

	SpinLock(const SpinLock&) = delete;
	SpinLock& operator= (const SpinLock&) = delete;

	void Lock() { m_lock.lock(); }
	void Unlock() { m_lock.unlock(); }
	bool TryLock() { return m_lock.try_lock(); }
private:
	std::mutex m_lock;
	
};

#endif //SPIN_LOCK_H

