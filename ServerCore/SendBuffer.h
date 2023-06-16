#pragma once
class SendBuffer
{
public:
	SendBuffer() = delete;
	SendBuffer(int bufferSize);
	~SendBuffer();

	// 버퍼 정보 반환
	inline BYTE*	GetBuffer()				{ return m_buffer.data(); }
	inline int		GetWriteSize() const	{ return m_writeSize; }
	inline int		GetCapacity() const		{ return static_cast<int>(m_buffer.size()); }

	void CopyData(void* data, int len);

private:
	vector<BYTE>	m_buffer;
	int				m_writeSize;
};