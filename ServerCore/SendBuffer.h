#pragma once
class SendBuffer
{
public:
	SendBuffer();
	~SendBuffer();

private:
	char*	m_buffer;
	int		m_writeSize;
};

