#include <algorithm>
#include <cstring>
#include "ring_buffer_dn.h"

RingBufferDN::RingBufferDN(BufferSize size)
{
	m_uIn = 0;
	m_uOut = 0;
	m_uSize = RoundUpPowOfTwo(size);
	m_pBuffer = new char[m_uSize];
}

RingBufferDN::~RingBufferDN()
{
	delete[] m_pBuffer;
	m_pBuffer = NULL;
}

BufferSize RingBufferDN::GetSize()
{
	return m_uSize;
}

BufferSize RingBufferDN::GetLen()
{
	BufferSize out = m_uOut;
	return m_uIn - out;
}

BufferSize RingBufferDN::GetAvail()
{
	return m_uSize - m_uIn + m_uOut;
}

bool RingBufferDN::IsEmpty()
{
	return m_uIn == m_uOut;
}

bool RingBufferDN::IsFull()
{
	return m_uSize == BufferSize(m_uIn - m_uOut);
}

void RingBufferDN::Reset()
{
	AutoLock<SpinLock> lock(&m_Lock);
	m_uIn = m_uOut = 0;
}

void RingBufferDN::ResetOut()
{
	AutoLock<SpinLock> lock(&m_Lock);
	m_uOut = m_uIn;
}

BufferSize RingBufferDN::ReadData(char *data, BufferSize len)
{
	AutoLock<SpinLock> lock(&m_Lock);
	len = std::min(BufferSize(m_uIn - m_uOut), len);
	BufferSize off = m_uOut  & (m_uSize - 1);
	BufferSize tail = m_uSize - off;
	BufferSize copy1 = std::min(len, tail);
	memcpy(data, m_pBuffer + off, copy1);
	memcpy(data + copy1, m_pBuffer, len - copy1);
	m_uOut += len;
	return len;
}

BufferSize RingBufferDN::WriteData(char *data, BufferSize len)
{
	AutoLock<SpinLock> lock(&m_Lock);
	len = std::min(BufferSize(m_uSize - m_uIn + m_uOut), len);
	BufferSize off = m_uIn  & (m_uSize - 1);
	BufferSize tail = m_uSize - off;
	BufferSize copy1 = std::min(len, tail);
	memcpy(m_pBuffer + off, data, copy1);
	memcpy(m_pBuffer, data + copy1, len - copy1);
	m_uIn += len;
	return len;
}