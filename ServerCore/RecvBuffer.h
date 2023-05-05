#pragma once
/*
	기존 버퍼를 최대한 재활용하는 구조로 메시지 수신
*/
class RecvBuffer
{
	enum { BUFFER_COUNT = 5 };

public:
	RecvBuffer() = delete;
	RecvBuffer(int bufferSize);
	~RecvBuffer();

	// 버퍼 조작
	void	Clear();
	bool	OnRead(int numberOfBytes);
	bool	OnWrite(int numberOfBytes);

	// 버퍼 정보 반환
	inline char*	GetReadPos()	{ return &m_buffer[m_readPos]; }
	inline char*	GetWritePos()	{ return &m_buffer[m_writePos]; }
	inline int		GetDataSize()	{ return m_writePos - m_readPos; }
	inline int		GetRemainSize() { return m_capacity - m_writePos; }
	inline int		GetBufferSize() { return m_bufferSize; }

private:
	int				m_capacity;		// 총 버퍼 용량
	int				m_bufferSize;	// 개당 버퍼 크기
	int				m_readPos;		// 읽기 위치
	int				m_writePos;		// 쓰기 위치
	char*			m_buffer;		// 수신 버퍼
};

