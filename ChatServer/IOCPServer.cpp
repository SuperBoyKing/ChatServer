#include "pch.h"
#include "IOCPServer.h"
#include "ClientSession.h"

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
	return;
}
