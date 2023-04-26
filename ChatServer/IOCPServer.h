#pragma once

class ClientSession;

class IOCPOperation;

class IIOCPBinder : public enable_shared_from_this<IIOCPBinder>
{
public:
	virtual SOCKET		GetSock() const = 0;
	virtual void		ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes = 0) = 0;
};

class IOCPServer
{
public:
	IOCPServer();

	~IOCPServer();

	HANDLE		GetIocpHandle() const { return m_iocpHandle; }
	bool		BindIOCompletionPort(IIOCPBinder& iocpBinder);
	bool		GetIOCompletionPort();

private:
	HANDLE		m_iocpHandle;
};

extern IOCPServer iocpServer; // юс╫ц

