#pragma once

class IOCPOperation; 

class ClientSession : public IIOCPBinder
{
public:
	ClientSession();
	ClientSession(SOCKET clientSocket);
	virtual ~ClientSession();

	// Override Function
	inline SOCKET		GetSock() const override	{ return m_socket; }
	void				ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes = 0) override;

public:

	bool		Connect();
	void		Disconnect();

	inline char*	GetRecvBuffer()				{ return m_recvBuffer; }
	inline int		GetRecvBufferSize() const	{ return m_recvSize; }

	inline char*	GetSendBuffer()				{ return m_sendBuffer; }
	inline int		GetSendBufferSize() const	{ return m_sendSize; }

	// Operation 처리
	void		ProcessSend(unsigned int numberOfBytes);
	void		ProcessRecv(unsigned int numberOfBytes);
	void		ProcessConnect();
	void		ProcessDisconnect();

	// Operation 등록
	void		RegisterSend();	// 임시
	void		RegisterRecv();
	void		RegisterConnect();
	void		RegisterDisconnect();

private:
	SOCKET			m_socket;
	IOCPOperation	m_sendOperation;
	IOCPOperation	m_recvOperation;

private:
	char			m_recvBuffer[MAX_BUFFER_SIZE];
	int				m_recvSize;
	char			m_sendBuffer[MAX_BUFFER_SIZE];
	int				m_sendSize;
};

