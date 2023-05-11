#include "pch.h"
#include "IOCPHandler.h"
#include "ChatSession.h"
#include "IOCPOperation.h"

IOCPHandler::IOCPHandler()
{
	m_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	ASSERT_CRASH(m_iocpHandle != INVALID_HANDLE_VALUE);
}

IOCPHandler::~IOCPHandler()
{
	::CloseHandle(m_iocpHandle);
}

bool IOCPHandler::BindIOCompletionPort(shared_ptr<IIOCPBinder> iocpBinder)
{
	return ::CreateIoCompletionPort((HANDLE)iocpBinder->GetSock(), m_iocpHandle, NULL, NULL);
}

void IOCPHandler::CallGQCS()
{
	DWORD bytesTransferred = 0;
	IOCPOperation* iocpOperation = nullptr;
	ChatSession* clientSession = nullptr;

	if (::GetQueuedCompletionStatus(m_iocpHandle, &bytesTransferred, (PULONG_PTR)&clientSession, reinterpret_cast<LPOVERLAPPED*>(&iocpOperation), INFINITE))
	{
		shared_ptr<IIOCPBinder> iocpBinder = iocpOperation->GetOwner();
		iocpBinder->ProcessOperation(iocpOperation, bytesTransferred);
	}
	else
	{
		int errorCode = ::WSAGetLastError();
		switch (errorCode)
		{
		case WAIT_TIMEOUT:
			PRINT_WSA_ERROR("GetQueuedCompletionStatus Error");
			return;
		default:
			shared_ptr<IIOCPBinder> iocpBinder = iocpOperation->GetOwner();
			iocpBinder->ProcessOperation(iocpOperation, bytesTransferred);
			break;
		}
	}
}
