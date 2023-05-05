#pragma once
#include "RecvBuffer.h"

class IOCPOperation; 

class ClientSession : public IIOCPBinder
{
	friend class ClientListener;

public:
	ClientSession();
	ClientSession(SOCKET clientSocket);
	virtual ~ClientSession();

	// Override Function
	inline SOCKET		GetSock() const override	{ return m_socket; }
	void				ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes = 0) override;

public:

	void		Connect();
	void		Disconnect();
	void		Send(const char* sendBuffer, const unsigned int len);

	inline char*	GetSendBuffer()				{ return m_sendBuffer; }
	inline int		GetSendBufferSize() const	{ return m_sendSize; }
	inline void		SetSendBufferSize(unsigned int size) { m_sendSize = size; } // temp

	inline bool		IsConnected() const			{ return m_isConnected; }

	// Operation 처리
	void		ProcessSend(SendOperation* sendOperation, unsigned int numberOfBytes);
	void		ProcessRecv(unsigned int numberOfBytes);
	void		ProcessConnect();
	void		ProcessDisconnect();

	// Operation 등록
	void		RegisterSend(SendOperation* sendOperation);	// 임시
	void		RegisterRecv();
	void		RegisterConnect();
	void		RegisterDisconnect();

	// Operation 재정의
public:
	virtual void		OnSend(unsigned int len) {};
	virtual void		OnRecv(char* buffer, unsigned int len) { cout << buffer << ", Len : " << len <<  endl;  };
	virtual void		OnConnect() {};
	virtual void		OnDisconnect() {};

private:
	SOCKET				m_socket;
	RecvOperation		m_recvOperation;
	ConnectOperation	m_connectOperation;
	DisconnectOperation	m_disconnectOperation;

	mutex				m_mutex;

private:
	//char			m_recvBuffer[MAX_BUFFER_SIZE];
	//int			m_recvSize;
	RecvBuffer		m_recvBuffer;
	char			m_sendBuffer[MAX_BUFFER_SIZE];
	int				m_sendSize;
	atomic<bool>	m_isConnected;
};

