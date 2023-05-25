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
		
	inline void		SetApp(weak_ptr<ChatApplication> chatApp)		{ m_chatApp = chatApp; }

	inline void		SetUserID(const char* userID, int size = 33)	{ ::memcpy(m_userID, userID, 33); }

	inline void		SetRoomNumber(const int roomNumber)				{ m_roomNumber = roomNumber; }

	inline const char*	GetUserID() const		{ return m_userID; }

	inline const int	GetRoomNumber() const	{ return m_roomNumber; }

	inline bool			IsConnected() const		{ return m_isConnected; }

	// Operation ó��
	void		ProcessSend(unsigned int numberOfBytes);
	void		ProcessRecv(unsigned int numberOfBytes);
	void		ProcessConnect();
	void		ProcessDisconnect();

	// Operation ���
	void		RegisterSend();
	void		RegisterRecv();
	bool		RegisterConnect();
	void		RegisterDisconnect();

	// Operation ������
protected:
	virtual void		OnSend(unsigned int len) {};
	virtual void		OnRecv(char* buffer, unsigned int len) {};
	virtual void		OnConnect() {};
	virtual void		OnDisconnect() {};

	// Operation ���
private:
	RecvOperation		m_recvOperation;
	SendOperation		m_sendOperation;
	ConnectOperation	m_connectOperation;
	DisconnectOperation	m_disconnectOperation;

	// ��/���� ó�� ���
private:
	RecvBuffer						m_recvBuffer;
	queue<shared_ptr<SendBuffer>>	m_sendQueue;
	atomic<bool>					m_isRegisteredSend;
	atomic<bool>					m_isConnected;

	weak_ptr<ChatApplication>		m_chatApp;

	// ChatSession ���
private:
	SOCKET				m_socket;
	recursive_mutex		m_mutex;
	char				m_userID[32 + 1];
	int					m_roomNumber;
};

