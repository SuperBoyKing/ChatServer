#pragma once
/*
	���� ���۸� �ִ��� ��Ȱ���ϴ� ������ �޽��� ����
*/
class RecvBuffer
{
	enum { BUFFER_COUNT = 5 };

public:
	RecvBuffer() = delete;
	RecvBuffer(int bufferSize);
	~RecvBuffer();

	// ���� ����
	void	Clear();
	bool	OnRead(int numberOfBytes);
	bool	OnWrite(int numberOfBytes);

	// ���� ���� ��ȯ
	inline char*	GetReadPos()	{ return &m_buffer[m_readPos]; }
	inline char*	GetWritePos()	{ return &m_buffer[m_writePos]; }
	inline int		GetDataSize()	{ return m_writePos - m_readPos; }
	inline int		GetRemainSize() { return m_capacity - m_writePos; }
	inline int		GetBufferSize() { return m_bufferSize; }

private:
	int				m_capacity;		// �� ���� �뷮
	int				m_bufferSize;	// ���� ���� ũ��
	int				m_readPos;		// �б� ��ġ
	int				m_writePos;		// ���� ��ġ
	char*			m_buffer;		// ���� ����
};

