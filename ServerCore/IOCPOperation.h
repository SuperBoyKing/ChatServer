#pragma once
#include "IOCPHandler.h"
#include "SendBuffer.h"

class ChatSession;

enum class OperationType : unsigned __int8
{
	NONE,
	ACCEPT,
	SEND,
	RECV,
	CONNECT,
	DISCONNECT,
	DB_RESPONSE,
};

class IOCPOperation : public OVERLAPPED
{
public:
	IOCPOperation() = default;
	IOCPOperation(OperationType operationType);

	void Init();

	inline void					SetType(OperationType OperationType)	{ m_operationType = OperationType; }
	inline OperationType		GetType() const							{ return m_operationType; }

	inline void			SetOwner(const shared_ptr<IIOCPBinder>& ptr)	{ m_owner = ptr; }
	inline void			ReleaseOwner()									{ m_owner = nullptr; }

	inline shared_ptr<IIOCPBinder> GetOwner() const						{ return m_owner; }

public:
	OperationType				m_operationType = OperationType::NONE;
	shared_ptr<IIOCPBinder>		m_owner;
};


// SendOperation
class SendOperation : public IOCPOperation
{
public:
	SendOperation() : IOCPOperation(OperationType::SEND) { sendBuffers.reserve(100); }

public:
	vector<shared_ptr<SendBuffer>> sendBuffers;
};

// RecvOperation
class RecvOperation : public IOCPOperation
{
public:
	RecvOperation() : IOCPOperation(OperationType::RECV) {}
};

// ConnectOperation
class ConnectOperation : public IOCPOperation
{
public:
	ConnectOperation() : IOCPOperation(OperationType::CONNECT) {}
};

// DisconnectOperation
class DisconnectOperation : public IOCPOperation
{
public:
	DisconnectOperation() : IOCPOperation(OperationType::DISCONNECT) {}
};

// AcceptOperation
class AcceptOperation : public IOCPOperation
{
public:
	AcceptOperation() : IOCPOperation(OperationType::ACCEPT) {}

	inline void							SetSession(shared_ptr<ChatSession> session)		{ m_acceptedClientSession = session; }
	inline shared_ptr<ChatSession>		GetSession() const								{ return m_acceptedClientSession; }

private:
	shared_ptr<ChatSession> m_acceptedClientSession = nullptr;
};

// DBOperation
class DBResOperation : public IOCPOperation
{
public:
	DBResOperation() : IOCPOperation(OperationType::DB_RESPONSE) {}

public:
	shared_ptr<SendBuffer> sendBuffer;
};