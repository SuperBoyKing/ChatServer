#pragma once
class IOCPServer
{
public:
	IOCPServer();

	~IOCPServer();

	HANDLE GetIocpHandle() const { return m_iocpHandle; }

	//bool BindIOCompletionPort(ClientSession)

private:
	HANDLE m_iocpHandle;
};

