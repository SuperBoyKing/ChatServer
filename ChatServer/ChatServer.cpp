#include "pch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"

int main()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numberOfProcessor = sysinfo.dwNumberOfProcessors;

	function<shared_ptr<ClientSession>(void)> clientSession = make_shared<ClientSession>;

	shared_ptr<ChatServer> chatServer = make_shared<ChatServer>(
		make_shared<ServerAddress>(L"127.0.0.1", SERVER_PORT),
		make_shared<IOCPHandler>(),
		clientSession,
		1);


	ASSERT_CRASH(chatServer->Start());

	for (int i = 0; i < numberOfProcessor; ++i)
	{
		GThreadManager->Launch([=]() {
			while (true)
			{
				chatServer->GetIOCPHandler()->CallGQCS();
			}
		});
	}
	
	cout << "Server Start!" << endl;

	/*const char* a = "Broadcast";

	while (true)
	{
		this_thread::sleep_for(1s);
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(MAX_SEND_BUFFER_SIZE);
		sendBuffer->CopyData((char*)a, sizeof(char) * 10);
		GClientSessionManager->Broadcast(sendBuffer);
	}*/

	GThreadManager->Join();

	return 0;
}