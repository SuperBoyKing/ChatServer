#include "pch.h"
#include "ThreadManager.h"

int main()
{
	this_thread::sleep_for(500ms);
	shared_ptr<ClientSession> clientSession = make_shared<ClientSession>();
	GIOCPHandler->BindIOCompletionPort(clientSession);
	clientSession->Connect();

	GThreadManager->Launch([=]() {
		while (true)
		{
			GIOCPHandler->CallGQCS();
		}
	});
	
	char sendBuffer[100] = {};
	while (true)
	{
		cin.getline(sendBuffer, 100);
		if (strcmp(sendBuffer, "exit") == 0)
		{
			clientSession->RegisterDisconnect();
			break;
		}
		clientSession->Send(sendBuffer, strnlen_s(sendBuffer, 100));
		memset(sendBuffer, 0, 100);
	}

	clientSession = nullptr;
	GThreadManager->Join();
	return 0;
}