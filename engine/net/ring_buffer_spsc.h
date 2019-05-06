#ifndef RING_BUFFER_SPSC_H
#define RING_BUFFER_SPSC_H

#include "ring_buffer_base.h"
#include "thread/auto_lock.h"

class RingBufferSpsc : public RingBufferBase
{
public:
	RingBufferSpsc(BufferSize size);
	virtual ~RingBufferSpsc();

	virtual BufferSize GetSize();
	virtual BufferSize GetLen();
	virtual BufferSize GetAvail();

	virtual bool IsEmpty();
	virtual bool IsFull();

	virtual void Reset();//²»°²È«;
	virtual void ResetOut();


	virtual BufferSize ReadData(char *data, BufferSize len);
	virtual BufferSize WriteData(char *data, BufferSize len);

	RingBufferSpsc(const RingBufferSpsc&) = delete;
	RingBufferSpsc& operator= (const RingBufferSpsc&) = delete;

private:
	std::atomic<BufferSize> m_uIn = { 0 };
	std::atomic<BufferSize> m_uOut = { 0 };
};

#endif   //RING_BUFFER_SPSC_H
