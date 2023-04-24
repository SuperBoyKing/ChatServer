#pragma once
class IOCPServer
{
public:
	IOCPServer();

	~IOCPServer();

	HANDLE GetIocpHandle() const { return m_iocpHandle; }

	bool BindIOCompletionPort(ClientSession& clientSession);

	bool GetIOCompletionPort();

private:
	HANDLE m_iocpHandle;
};

