#include "pch.h"

char recvBuffer[1000] = {  };

void getError(const char* reason, int cause)
{
	cout << reason << " : " << cause << endl;
}

int main()
{
	ServerAddress serverAddress(L"127.0.0.1", SERVER_PORT);
	SocketAssistant::Init();
	SOCKET serverSocket = SocketAssistant::CreateSocket();
	SocketAssistant::SetBind(serverSocket, serverAddress.GetSockAddrIn());
	SocketAssistant::SetListen(serverSocket);

	SOCKET clientSocket = INVALID_SOCKET;
	sockaddr_in clientAddr;
	int addrLen = sizeof(sockaddr_in);
	clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &addrLen);
	if (clientSocket == INVALID_SOCKET)
	{
		getError("Accept Error", WSAGetLastError());
		closesocket(clientSocket);
		closesocket(serverSocket);
		WSACleanup();
	}

	cout << "Client Connected!" << endl;
	while (true)
	{
		recv(clientSocket, recvBuffer, sizeof(recvBuffer), 0);
		cout << recvBuffer << endl;
		this_thread::sleep_for(250ms);
	}

	return 0;
}