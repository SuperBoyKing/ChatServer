#include "pch.h"

int main()
{
	this_thread::sleep_for(500ms);
	shared_ptr<ChatClient> chatClient = make_shared<ChatClient>(
		make_shared<ServerAddress>(L"127.0.0.1", SERVER_PORT),
		make_shared<IOCPHandler>(), 
		100);

	ASSERT_CRASH(chatClient->Start());

	GThreadManager->Launch([=]() {
		for (int i = 0; i < 5; ++i);
			while (true)
			{
				chatClient->GetIOCPHandler()->CallGQCS();
			}
	});
	
	const char* a = "HelloWorld";

	while (true)
	{
		this_thread::sleep_for(1s);
		shared_ptr<SendBuffer> sendBuf = make_shared<SendBuffer>(MAX_SEND_BUFFER_SIZE);
		sendBuf->CopyData((char*)a, sizeof(char) * 11);
		GClientSessionManager->Broadcast(sendBuf);
	}

	GThreadManager->Join();
	return 0;
}