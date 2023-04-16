#include "pch.h"

int main()
{
	this_thread::sleep_for(1s);

	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
	{
		WSACleanup();
		return 0;
	}

	SOCKET clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	bool flag = true;
	setsockopt(clientSock, SOL_SOCKET, SO_REUSEADDR, (char*)&flag, sizeof(flag));

	SOCKADDR_IN sockaddr;
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(2023);

	if (inet_pton(AF_INET, "127.0.0.1", &sockaddr.sin_addr) <= 0)
	{
		cout << "Invalid address" << endl;
		return 0;
	}

	while (true)
	{
		int error = connect(clientSock, (SOCKADDR*)&sockaddr, sizeof(SOCKADDR));
		if (WSAGetLastError() == WSAEWOULDBLOCK)
		{
			continue;
		}
		else if (error == SOCKET_ERROR)
		{
			cout << "Connect Error : " << WSAGetLastError() << endl;
			closesocket(clientSock);
			WSACleanup();
			return 0;
		}
		else
		{
			cout << "Connect Success!" << endl;
			break;
		}
	}
	

	char buffer[100] = {};

	while (true)
	{
		cout << "send Msg : ";
		cin.getline(buffer, sizeof(buffer), '\n');
		int sendBytes = ::send(clientSock, buffer, sizeof(buffer), 0);
		if (sendBytes != SOCKET_ERROR)
		{
			cout << "send Bytes : " << sendBytes << endl;
		}
		else
		{
			cout << "send Error" << endl;
			break;
		}

	//	this_thread::sleep_for(1s);
	}

	return 0;
}