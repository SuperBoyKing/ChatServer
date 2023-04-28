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
		memcpy(clientSession->GetSendBuffer(), recvBuffer, sizeof(char) * MAX_BUFFER_SIZE);
		clientSession->RegisterSend();
	}

	GThreadManager->Join();
	clientSession = nullptr;
	return 0;
}