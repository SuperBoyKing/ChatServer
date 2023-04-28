#pragma once

class ClientSession;

class IOCPOperation;

class IIOCPBinder : public enable_shared_from_this<IIOCPBinder>
{
public:
	virtual SOCKET		GetSock() const = 0;
	virtual void		ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes = 0) = 0;
};

class IOCPHandler
{
public:
	IOCPHandler();

	~IOCPHandler();

	HANDLE		GetIocpHandle() const { return m_iocpHandle; }
	bool		BindIOCompletionPort(shared_ptr<IIOCPBinder> iocpBinder);
	bool		CallGQCS();

private:
	HANDLE		m_iocpHandle;
};

