#ifndef RING_BUFFER_BASE_H
#define RING_BUFFER_BASE_H

typedef unsigned char BufferSize;

class RingBufferBase
{
public:
	RingBufferBase() {}
	RingBufferBase(BufferSize size){}
	virtual ~RingBufferBase() {}

	RingBufferBase(const RingBufferBase&) = delete;
	RingBufferBase& operator= (const RingBufferBase&) = delete;

	virtual BufferSize GetSize() = 0;
	virtual BufferSize GetLen() = 0;
	virtual BufferSize GetAvail() = 0;

	virtual bool IsEmpty() = 0;
	virtual bool IsFull() = 0;

	virtual void Reset() = 0;
	virtual void ResetOut() = 0;


	virtual BufferSize ReadData(char *data, BufferSize len) = 0;
	virtual BufferSize WriteData(char *data, BufferSize len) = 0;

protected:
	static bool IsPowerOf2(BufferSize val) { return (val != 0 && (val && (val - 1)) == 0); }
	static BufferSize RoundUpPowOfTwo(BufferSize val)
	{
		val -= 1;
		int pos = 1;
		BufferSize i;
		if (val != 0)
		{
			for (i = val >> 1; i != 0; ++pos)
			{
				i >>= 1;
			}
		}
		else
		{
			pos -= 1;
		}
		return BufferSize(1 << pos);
	}
protected:
	BufferSize m_uSize;	
	char *m_pBuffer;
};

#endif   //RING_BUFFER_BASE_H
