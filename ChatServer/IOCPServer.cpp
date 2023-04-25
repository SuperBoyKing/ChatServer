#include "pch.h"
#include "IOCPServer.h"
#include "ClientSession.h"
#include "IOCPOperation.h"

IOCPServer::IOCPServer()
{
	m_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	ASSERT_CRASH(m_iocpHandle != INVALID_HANDLE_VALUE);
}

IOCPServer::~IOCPServer()
{
	::CloseHandle(m_iocpHandle);
}

bool IOCPServer::BindIOCompletionPort(ClientSession& clientSession)
{
	return ::CreateIoCompletionPort((HANDLE)clientSession.GetSock(), m_iocpHandle, (ULONG_PTR)&clientSession, NULL);
}

bool IOCPServer::GetIOCompletionPort()
{
	DWORD bytesTransferred = 0;
	IOCPOperation* iocpOperation = nullptr;
	ClientSession* clientSession = nullptr;

	if (::GetQueuedCompletionStatus(m_iocpHandle, &bytesTransferred, (PULONG_PTR)&clientSession, reinterpret_cast<LPOVERLAPPED*>(&iocpOperation), INFINITE))
	{
		WSABUF wsaBuf;
		wsaBuf.buf = clientSession->GetRecvBuffer();
		wsaBuf.len = MAX_RECV_BUFFER;

		cout << clientSession->GetRecvBuffer() << ", Length : " << bytesTransferred << endl;

		DWORD recvLen = 0;
		DWORD key = 0;
		if (iocpOperation->GetType() == IoType::RECV)
		{
			if (::WSARecv(clientSession->GetSock(), &wsaBuf, 1, &recvLen, &key, iocpOperation, NULL))
			{
				int errorCode = ::WSAGetLastError();
				if (errorCode != WSA_IO_PENDING)
				{
					LOG_CALL("Handle Error", errorCode);
					return false;
				}
			}
		}
	}
	else
	{
		LOG_CALL("GetQueuedCompletionStatus Error", ::WSAGetLastError());
		return false;
	}
	
	return true;
}
