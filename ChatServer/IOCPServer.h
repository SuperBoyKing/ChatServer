#pragma once

class ClientSession;

class IOCPOperation;

class IOCPBinder
{
public:
	virtual SOCKET		GetSock() const abstract;
	virtual void		ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes = 0) abstract;
};

class IOCPServer
{
public:
	IOCPServer();

	~IOCPServer();

	HANDLE		GetIocpHandle() const { return m_iocpHandle; }
	bool		BindIOCompletionPort(ClientSession& clientSession);
	bool		GetIOCompletionPort();

private:
	HANDLE		m_iocpHandle;
};

