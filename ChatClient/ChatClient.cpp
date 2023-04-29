#include "pch.h"
#include "ThreadManager.h"

int main()
{
	this_thread::sleep_for(500ms);
	shared_ptr<ClientSession> clientSession = make_shared<ClientSession>();
	clientSession->Connect();

	GThreadManager->Launch([=]() {
		while (true)
		{
			GIOCPHandler->CallGQCS();
		}
	});
	
	char recvBuffer[MAX_BUFFER_SIZE] = {};

	while (true)
	{
		cin.getline(recvBuffer, sizeof(char) * MAX_BUFFER_SIZE);
		if (strcmp(recvBuffer, "exit") == 0)
		{
			clientSession->RegisterDisconnect();
			break;
		}
		clientSession->SetSendBufferSize(strlen(recvBuffer) + 1);

		memcpy(clientSession->GetSendBuffer(), recvBuffer, sizeof(char) * clientSession->GetSendBufferSize());
		clientSession->RegisterSend();
	}

	clientSession = nullptr;
	GThreadManager->Join();
	return 0;
}