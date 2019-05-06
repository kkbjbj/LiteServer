#ifndef RING_BUFFER_DN_H
#define RING_BUFFER_DN_H

#include "ring_buffer_base.h"
#include "thread/auto_lock.h"


class RingBufferDN : public RingBufferBase
{
public:
	RingBufferDN(BufferSize size);
	virtual ~RingBufferDN();

	virtual BufferSize GetSize();
	virtual BufferSize GetLen();
	virtual BufferSize GetAvail();

	virtual bool IsEmpty();
	virtual bool IsFull();

	virtual void Reset();
	virtual void ResetOut();


	virtual BufferSize ReadData(char *data, BufferSize len);
	virtual BufferSize WriteData(char *data, BufferSize len);

	RingBufferDN(const RingBufferDN&) = delete;
	RingBufferDN& operator= (const RingBufferDN&) = delete;

private:
	BufferSize m_uIn;
	BufferSize m_uOut;
	SpinLock m_Lock;
};



#endif //RING_BUFFER_DN_H
