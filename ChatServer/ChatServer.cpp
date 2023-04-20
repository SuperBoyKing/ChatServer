#include "pch.h"

char recvBuffer[1000] = {  };

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

class Client
{
public:
	Client(SOCKET socket) : m_socket(socket)
	{}

	SOCKET GetSocket() const { return m_socket; }

public:
	SOCKET m_socket;
	char m_recvBuffer[1000] = {};
	int  m_recvSize = 0;
};

void WorkerThread(HANDLE iocpHandle)
{
	while (true)
	{
		DWORD bytesTransferred = 0;
		stOverlappedEx* overlappedEx = nullptr;
		Client* clientInfo = nullptr;

		bool ret = ::GetQueuedCompletionStatus(iocpHandle, OUT &bytesTransferred, 
			OUT (PULONG_PTR)&clientInfo, OUT (LPOVERLAPPED*)& overlappedEx, INFINITE);

		if (ret == FALSE || bytesTransferred == 0)
			continue;

		ASSERT_CRASH(overlappedEx->ioType == IO_TYPE::RECV)

		cout << clientInfo->m_recvBuffer <<", Length : " << bytesTransferred << endl;

		ZeroMemory(clientInfo->m_recvBuffer, 1000);

		WSABUF wsaBuf;
		wsaBuf.buf = clientInfo->m_recvBuffer;
		wsaBuf.len = 1000;

		DWORD recvLen = 0;
		DWORD flags = 0;
		::WSARecv(clientInfo->m_socket, &wsaBuf, 1, &recvLen, &flags, &overlappedEx->overlapped, NULL);
	}
}

int main()
{
	ServerAddress serverAddress(L"127.0.0.1", SERVER_PORT);
	SocketAssistant::Init();
	SOCKET acceptSocket = SocketAssistant::CreateSocket();
	SocketAssistant::SetLinger(acceptSocket, 0, 0);
	SocketAssistant::SetReuseAddress(acceptSocket);
	SocketAssistant::SetBind(acceptSocket, serverAddress.GetSockAddrIn());
	SocketAssistant::SetListen(acceptSocket);
	
	HANDLE iocpHandle = ::CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, NULL, NULL);
	::thread th(WorkerThread, iocpHandle);

	while (true)
	{
		SOCKADDR_IN clientAddr;
		int addrLen = sizeof(SOCKADDR_IN);
		SOCKET clientSocket = ::accept(acceptSocket, (SOCKADDR*)&clientAddr, &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			cout << "Accept Socket Error" << endl;
			continue;
		}

		cout << "Accept Client" << endl;

		Client* clientInfo = new Client(clientSocket);
		::CreateIoCompletionPort((HANDLE)clientSocket, iocpHandle, (ULONG_PTR)clientInfo, 0);
		
		WSABUF wsaBuf;
		wsaBuf.buf = clientInfo->m_recvBuffer;
		wsaBuf.len = 1000;

		stOverlappedEx* overlappedEx = new stOverlappedEx();
		overlappedEx->ioType = IO_TYPE::RECV;
		
		DWORD receivedBytes = 0;
		DWORD flags = 0;
		::WSARecv(clientSocket, &wsaBuf, 1, &receivedBytes, &flags, &overlappedEx->overlapped, NULL);
	}

	if (th.joinable())
		th.join();

	return 0;
}