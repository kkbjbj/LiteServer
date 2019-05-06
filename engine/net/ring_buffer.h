#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include "ring_buffer_base.h"
#include "thread/auto_lock.h"


class RingBuffer : public RingBufferBase
{
public:
	RingBuffer(BufferSize size);
	virtual ~RingBuffer();

	virtual BufferSize GetSize();
	virtual BufferSize GetLen();
	virtual BufferSize GetAvail();

	virtual bool IsEmpty();
	virtual bool IsFull();

	virtual void Reset();
	virtual void ResetOut();


	virtual BufferSize ReadData(char *data, BufferSize len);
	virtual BufferSize WriteData(char *data, BufferSize len);

	RingBuffer(const RingBuffer&) = delete;
	RingBuffer& operator= (const RingBuffer&) = delete;

private:
	BufferSize m_uIn;
	BufferSize m_uOut;
	AtomSpinLock m_Lock;
};



#endif //RING_BUFFER_H
