#include "pch.h"
#include "ThreadManager.h"

int main()
{
	this_thread::sleep_for(500ms);
	shared_ptr<ClientSession> clientSession = make_shared<ClientSession>();
	GIOCPHandler->BindIOCompletionPort(clientSession);
	clientSession->Connect();

	GThreadManager->Launch([=]() {
		for (int i = 0; i < 5; ++i);
			while (true)
			{
				GIOCPHandler->CallGQCS();
			}
	});
	
	const char* a = "HelloWorld";

	while (true)
	{
		this_thread::sleep_for(250ms);
		shared_ptr<SendBuffer> sendBuf = make_shared<SendBuffer>(MAX_SEND_BUFFER_SIZE);
		sendBuf->CopyData((char*)a, sizeof(char) * 11);
		clientSession->Send(sendBuf);
	}

	clientSession = nullptr;
	GThreadManager->Join();
	return 0;
}