#include "pch.h"
#include "SendBuffer.h"

SendBuffer::SendBuffer(int bufferSize)
	:m_writeSize(0)
{
	m_buffer.resize(bufferSize);
}

SendBuffer::~SendBuffer()
{
}

void SendBuffer::CopyData(void* data, int len)
{
	ASSERT_CRASH(GetCapacity() >= len);
	::memcpy(m_buffer.data(), data, len);
	m_writeSize = len;
}
