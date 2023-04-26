#include "pch.h"
#include "IOCPServer.h"
#include "ClientSession.h"
#include "IOCPOperation.h"

IOCPServer iocpServer;

IOCPServer::IOCPServer()
{
	m_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	ASSERT_CRASH(m_iocpHandle != INVALID_HANDLE_VALUE);
}

IOCPServer::~IOCPServer()
{
	::CloseHandle(m_iocpHandle);
}

bool IOCPServer::BindIOCompletionPort(IIOCPBinder& iocpBinder)
{
	return ::CreateIoCompletionPort((HANDLE)iocpBinder.GetSock(), m_iocpHandle, NULL, NULL);
}

bool IOCPServer::GetIOCompletionPort()
{
	DWORD bytesTransferred = 0;
	IOCPOperation* iocpOperation = nullptr;
	ClientSession* clientSession = nullptr;

	if (::GetQueuedCompletionStatus(m_iocpHandle, &bytesTransferred, (PULONG_PTR)&clientSession, reinterpret_cast<LPOVERLAPPED*>(&iocpOperation), INFINITE))
	{
		shared_ptr<IIOCPBinder> iocpBinder = iocpOperation->GetOwner();
		iocpBinder->ProcessOperation(iocpOperation, bytesTransferred);
	}
	else
	{
		PRINT_WSA_ERROR("GetQueuedCompletionStatus Error", ::WSAGetLastError());
		return false;
	}
	
	return true;
}
