#include <algorithm>
#include <cstring>

#include "ring_buffer_spsc.h"

RingBufferSpsc::RingBufferSpsc(BufferSize size)
{
	m_uSize = RoundUpPowOfTwo(size);
	m_pBuffer = new char[m_uSize];
}

RingBufferSpsc::~RingBufferSpsc()
{
	delete[] m_pBuffer;
	m_pBuffer = NULL;
}

BufferSize RingBufferSpsc::GetSize()
{
	return m_uSize;
}

BufferSize RingBufferSpsc::GetLen()
{
	const BufferSize outpos = m_uOut.load(std::memory_order_relaxed);
	const BufferSize inpos = m_uIn.load(std::memory_order_acquire);
	return inpos - outpos;
}

BufferSize RingBufferSpsc::GetAvail()
{
	const BufferSize outpos = m_uOut.load(std::memory_order_acquire);
	const BufferSize inpos = m_uIn.load(std::memory_order_relaxed);
	return m_uSize - inpos + outpos;
}

bool RingBufferSpsc::IsEmpty()
{
	const BufferSize outpos = m_uOut.load(std::memory_order_relaxed);
	const BufferSize inpos = m_uIn.load(std::memory_order_relaxed);
	return inpos == outpos;
}

bool RingBufferSpsc::IsFull()
{
	const BufferSize outpos = m_uOut.load(std::memory_order_relaxed);
	const BufferSize inpos = m_uIn.load(std::memory_order_relaxed);
	return m_uSize == BufferSize(inpos - outpos);
}

void RingBufferSpsc::Reset()
{
	m_uIn.store(0);
	m_uOut.store(0);
}

void RingBufferSpsc::ResetOut()
{
	const BufferSize outpos = m_uOut.load(std::memory_order_relaxed);
	const BufferSize inpos = m_uIn.load(std::memory_order_acquire);
	m_uOut.store(inpos, std::memory_order_release);
}

BufferSize RingBufferSpsc::ReadData(char *data, BufferSize len)
{
	const BufferSize outpos = m_uOut.load(std::memory_order_relaxed);
	const BufferSize inpos = m_uIn.load(std::memory_order_acquire);	

	len = std::min(BufferSize(inpos - outpos), len);
	if (len == 0) return len;
	BufferSize off = outpos  & (m_uSize - 1);
	BufferSize tail = m_uSize - off;
	BufferSize copy1 = std::min(len, tail);
	memcpy(data, m_pBuffer + off, copy1); 
	memcpy(data + copy1, m_pBuffer, len - copy1);
	m_uOut.store(outpos + len, std::memory_order_release);
	return len;
}

BufferSize RingBufferSpsc::WriteData(char *data, BufferSize len)
{
	const BufferSize inpos = m_uIn.load(std::memory_order_relaxed);
	const BufferSize outpos = m_uOut.load(std::memory_order_acquire);

	len = std::min(BufferSize(m_uSize - inpos + outpos), len);
	if (len == 0) return len;
	BufferSize off = inpos  & (m_uSize - 1);
	BufferSize tail = m_uSize - off;
	BufferSize copy1 = std::min(len, tail);
	memcpy(m_pBuffer + off, data, copy1);
	memcpy(m_pBuffer, data + copy1, len - copy1);
	m_uIn.store(inpos + len, std::memory_order_release);
	return len;
}