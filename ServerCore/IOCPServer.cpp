#include "pch.h"
#include "IOCPServer.h"

IOCPServer::IOCPServer()
{
	m_iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	ASSERT_CRASH(m_iocpHandle != INVALID_HANDLE_VALUE);
}

IOCPServer::~IOCPServer()
{
	::CloseHandle(m_iocpHandle);
}
