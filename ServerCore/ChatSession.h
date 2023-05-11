#pragma once
#include "RecvBuffer.h"
#include "SendBuffer.h"

class ChatApplication;

class ChatSession : public IIOCPBinder
{
	friend class ClientListener;

public:
	ChatSession();
	virtual ~ChatSession();

	// Override Function
	inline SOCKET		GetSock() const override	{ return m_socket; }
	void				ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes = 0) override;

public:

	bool		Connect();
	void		Disconnect();
	void		Send(shared_ptr<SendBuffer> sendbuffer);
	inline void	SetApp(weak_ptr<ChatApplication> chatApp) { m_chatApp = chatApp; }

	inline bool		IsConnected() const			{ return m_isConnected; }

	// Operation 처리
	void		ProcessSend(unsigned int numberOfBytes);
	void		ProcessRecv(unsigned int numberOfBytes);
	void		ProcessConnect();
	void		ProcessDisconnect();

	// Operation 등록
	void		RegisterSend();
	void		RegisterRecv();
	bool		RegisterConnect();
	void		RegisterDisconnect();

	// Operation 재정의
public:
	virtual void		OnSend(unsigned int len) { cout << "Send Complete" << endl; };
	virtual void		OnRecv(char* buffer, unsigned int len) { cout << buffer << endl; };
	virtual void		OnConnect() {};
	virtual void		OnDisconnect() {};

private:
	SOCKET				m_socket;
	RecvOperation		m_recvOperation;
	SendOperation		m_sendOperation;
	ConnectOperation	m_connectOperation;
	DisconnectOperation	m_disconnectOperation;

	recursive_mutex		m_mutex;

private:
	RecvBuffer						m_recvBuffer;
	queue<shared_ptr<SendBuffer>>	m_sendQueue;
	atomic<bool>					m_isRegisteredSend;
	atomic<bool>					m_isConnected;

	weak_ptr<ChatApplication>		m_chatApp;
};

