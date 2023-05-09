#include "pch.h"

int main()
{
	shared_ptr<ChatServer> chatServer = make_shared<ChatServer>(
		make_shared<ServerAddress>(L"127.0.0.1", SERVER_PORT),
		make_shared<IOCPHandler>(), 
		100);

	ASSERT_CRASH(chatServer->Start());

	GThreadManager->Launch(
		[=]() {
			while (true)
			{
				chatServer->GetIOCPHandler()->CallGQCS();
			}
		});

	cout << "Server Start!" << endl;

	const char* a = "Broadcast";

	while (true)
	{
		this_thread::sleep_for(1s);
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(MAX_SEND_BUFFER_SIZE);
		sendBuffer->CopyData((char*)a, sizeof(char) * 10);
		GClientSessionManager->Broadcast(sendBuffer);
	}

	GThreadManager->Join();

	return 0;
}