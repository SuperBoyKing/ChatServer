#include "pch.h"
#include "RecvBuffer.h"

RecvBuffer::RecvBuffer(int bufferSize)
	: m_bufferSize(bufferSize)
	, m_readPos(0)
	, m_writePos(0)
{
	m_capacity = bufferSize * BUFFER_COUNT;
	m_buffer = new char[m_capacity]();
}

RecvBuffer::~RecvBuffer()
{
	delete m_buffer;
}

void RecvBuffer::Clear()
{
	int dataSize = GetDataSize();
	
	if (dataSize == 0)
	{
		m_writePos = 0;
		m_readPos = 0;
	}
	else
	{
		if (m_bufferSize > GetRemainSize())
		{
			::memcpy(&m_buffer[0], &m_buffer[m_readPos], dataSize);
			m_readPos = 0;
			m_writePos = GetDataSize();
		}
	}
}

bool RecvBuffer::OnRead(int numberOfBytes)
{
	if (numberOfBytes > GetDataSize())
		return false;

	m_readPos += numberOfBytes;
	return true;
}

bool RecvBuffer::OnWrite(int numberOfBytes)
{
	if (numberOfBytes > GetRemainSize())
		return false;

	m_writePos += numberOfBytes;
	return true;
}