#pragma once
#include "IOCPHandler.h"

enum class OperationType : unsigned __int8
{
	NONE,
	ACCEPT,
	SEND,
	RECV,
	CONNECT,
	DISCONNECT,
};

class IOCPOperation : public OVERLAPPED
{
public:
	IOCPOperation() = default;
	IOCPOperation(OperationType OperationType);

	void Init();

	inline void					SetType(OperationType OperationType)	{ m_ioType = OperationType; }
	inline OperationType		GetType() const							{ return m_ioType; }

	inline void			SetOwner(shared_ptr<IIOCPBinder> ptr)	{ m_owner = ptr; }
	inline void			ReleaseOwner()							{ m_owner = nullptr; }

	inline shared_ptr<IIOCPBinder> GetOwner() const				{ return m_owner; }

private:
	OperationType				m_ioType = OperationType::NONE;
	shared_ptr<IIOCPBinder>		m_owner;
};

