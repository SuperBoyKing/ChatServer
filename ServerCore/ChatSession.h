#pragma once
#include "RecvBuffer.h"
#include "SendBuffer.h"

class ChatApplication;

enum class SessionState : unsigned __int8
{
	NONE,
	LOGIN,
	ROOM
};

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
	bool		Send(shared_ptr<SendBuffer>& sendbuffer);
		
	inline void		SetApp(weak_ptr<ChatApplication> chatApp)		{ m_chatApp = chatApp; }

	inline void		SetUserID(const char* userID, int size = 33)	{ ::memcpy(m_userID, userID, size); }

	inline void		SetRoomNumber(const int roomNumber)				{ m_roomNumber = roomNumber; }

	inline void		SetSessionState(SessionState state)				{ m_sessionState = state; }

	inline const char*	GetUserID() const		{ return m_userID; }

	inline const int	GetRoomNumber() const	{ return m_roomNumber; }

	inline bool			IsConnected() const		{ return m_isConnected; }

	inline SessionState	GetSessionState() const { return m_sessionState; }

	// Operation 처리
	void		ProcessSend(unsigned int numberOfBytes);
	void		ProcessRecv(unsigned int numberOfBytes);
	void		ProcessConnect();
	void		ProcessDisconnect();
	void		ProcessDBResponse(DBResOperation* dbOperation, unsigned int numberOfBytes);

	// Operation 등록
	bool		RegisterSend();
	void		RegisterRecv();
	bool		RegisterConnect();
	void		RegisterDisconnect();

	// Operation 재정의
protected:
	virtual void		OnSend(BYTE* sendData) {};
	virtual void		OnRecv(char* buffer, unsigned int len) {};
	virtual void		OnConnect() {};
	virtual void		OnDisconnect() {};

	// Operation 멤버
private:
	RecvOperation		m_recvOperation;
	SendOperation		m_sendOperation;
	ConnectOperation	m_connectOperation;
	DisconnectOperation	m_disconnectOperation;

	// 송/수신 처리 멤버
private:
	RecvBuffer						m_recvBuffer;
	queue<shared_ptr<SendBuffer>>	m_sendQueue;
	atomic<bool>					m_isRegisteredSend;
	atomic<bool>					m_isConnected;

	weak_ptr<ChatApplication>		m_chatApp;

	// ChatSession 멤버
private:
	SOCKET				m_socket;
	recursive_mutex		m_sendQueueMutex;
	char				m_userID[32 + 1];
	int					m_roomNumber;
	SessionState		m_sessionState;
};

