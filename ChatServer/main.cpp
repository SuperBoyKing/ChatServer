#include "pch.h"
#include "ThreadManager.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"

void getError(const char* reason, int cause)
{
	cout << reason << " : " << cause << endl;
}

enum class IO_TYPE
{
	SEND,
	RECV
};

struct stOverlappedEx
{
	OVERLAPPED overlapped;
	IO_TYPE ioType;
};

void WorkerThread(HANDLE iocpHandle)
{
	while (true)
	{
		DWORD bytesTransferred = 0;
		stOverlappedEx* overlappedEx = nullptr;
		ClientSession* clientSession = nullptr;

		bool ret = ::GetQueuedCompletionStatus(iocpHandle, OUT &bytesTransferred, 
			OUT (PULONG_PTR)&clientSession, OUT (LPOVERLAPPED*)& overlappedEx, INFINITE);

		if (ret == FALSE || bytesTransferred == 0)
			continue;

		ASSERT_CRASH(overlappedEx->ioType == IO_TYPE::RECV)

		cout << clientSession->GetRecvBuffer() <<", Length : " << bytesTransferred << endl;

		ZeroMemory(clientSession->GetRecvBuffer(), MAX_RECV_BUFFER);

		WSABUF wsaBuf;
		wsaBuf.buf = clientSession->GetRecvBuffer();
		wsaBuf.len = 1000;

		DWORD recvLen = 0;
		DWORD flags = 0;
		::WSARecv(clientSession->GetSock(), &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	}
}

int main()
{
	ServerAddress serverAddress(L"127.0.0.1", SERVER_PORT);
	SocketAssistant::Init();
	SOCKET listenSocket = SocketAssistant::CreateSocket();
	SocketAssistant::SetLinger(listenSocket, 0, 0);
	SocketAssistant::SetReuseAddress(listenSocket);
	SocketAssistant::SetBind(listenSocket, serverAddress.GetSockAddrIn());
	SocketAssistant::SetListen(listenSocket);
	
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	GThreadManager->Launch(
		[=](int) {
			WorkerThread(iocpHandle);
		}, (int)iocpHandle);


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

		ClientSession* clientSession = new ClientSession(clientSocket);
		SocketAssistant::SetUpdateClientSocket(clientSession->GetSock(), listenSocket);
		GClientSessionManager->Add(*clientSession);

		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, (ULONG_PTR)clientSession, 0);
		
		WSABUF wsaBuf;
		wsaBuf.buf = clientSession->GetRecvBuffer();
		wsaBuf.len = MAX_RECV_BUFFER;

		stOverlappedEx* overlappedEx = new stOverlappedEx();
		overlappedEx->ioType = IO_TYPE::RECV;
		
		DWORD receivedBytes = 0;
		DWORD flags = 0;
		::WSARecv(clientSocket, &wsaBuf, 1, &receivedBytes, &flags, &overlappedEx->overlapped, NULL);
	}

	GThreadManager->Join();

	return 0;
}