#include "pch.h"
#include "ThreadManager.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "IOCPServer.h"
#include "IOCPBinder.h"

IOCPServer iocpServer;

int main()
{
	ServerAddress serverAddress(L"127.0.0.1", SERVER_PORT);
	SOCKET listenSocket = SocketAssistant::CreateSocket();
	SocketAssistant::SetLinger(listenSocket, 0, 0);
	SocketAssistant::SetReuseAddress(listenSocket);
	SocketAssistant::SetBind(listenSocket, serverAddress.GetSockAddrIn());
	SocketAssistant::SetListen(listenSocket);
	
	GThreadManager->Launch(
		[=]() {
			while (true)
			{ 
				iocpServer.GetIOCompletionPort();
			}
		});

	ClientSession* clientSession = nullptr;

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(SOCKADDR_IN);
		SOCKET clientSocket = ::accept(listenSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Accept Socket Error" << endl;
			continue;
		}

		cout << "Accept Client" << endl;

		clientSession = new ClientSession(clientSocket);
		SocketAssistant::SetUpdateClientSocket(clientSession->GetSock(), listenSocket);
		GClientSessionManager->Add(*clientSession);

		iocpServer.BindIOCompletionPort(*clientSession);
		
		WSABUF wsaBuf;
		wsaBuf.buf = clientSession->GetRecvBuffer();
		wsaBuf.len = MAX_RECV_BUFFER;

		IOCPBinder* iocpBinder = new IOCPBinder(IoOperation::RECV);
		
		DWORD receivedBytes = 0;
		DWORD flags = 0;
		::WSARecv(clientSocket, &wsaBuf, 1, &receivedBytes, &flags, reinterpret_cast<OVERLAPPED*>(iocpBinder), NULL);
	}

	GThreadManager->Join();
	delete clientSession;

	return 0;
}