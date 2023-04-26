#include "pch.h"
#include "ThreadManager.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "IOCPServer.h"
#include "IOCPOperation.h"
#include "ClientListener.h"

int main()
{
	ClientListener clientListener(L"127.0.0.1", SERVER_PORT);
	
	GThreadManager->Launch(
		[=]() {
			while (true)
			{ 
				iocpServer.GetIOCompletionPort();
			}
		});

	clientListener.ProcessAccept();

	GThreadManager->Join();

	return 0;
}