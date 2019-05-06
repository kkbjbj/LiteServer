#ifndef ATOM_SPIN_LOCK_H
#define ATOM_SPIN_LOCK_H

#include <atomic>
#include <mutex>

class AtomSpinLock
{
public:
	AtomSpinLock(){ }
	~AtomSpinLock() {}

	AtomSpinLock(const AtomSpinLock&) = delete;
	AtomSpinLock& operator= (const AtomSpinLock&) = delete;

	void Lock()
	{
		while (m_flag.test_and_set(std::memory_order_acquire)); //std::memory_order_acquire
	}
	void Unlock()
	{
		m_flag.clear(std::memory_order_release);//std::memory_order_release
	}
	bool TryLock()
	{
		return m_flag.test_and_set();
	}
private:
	std::atomic_flag m_flag = ATOMIC_FLAG_INIT;
};

//std::lock_guard<AtomSpinLock> c;

#endif //ATOM_SPIN_LOCK_H

