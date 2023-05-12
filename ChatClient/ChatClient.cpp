#include "pch.h"
#include "ServerSession.h"

int main()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numberOfProcessor = sysinfo.dwNumberOfProcessors;

	function<shared_ptr<ServerSession>(void)> chatSession = make_shared<ServerSession>;

	this_thread::sleep_for(500ms);
	shared_ptr<ChatClient> chatClient = make_shared<ChatClient>(
		make_shared<ServerAddress>(L"127.0.0.1", SERVER_PORT),
		make_shared<IOCPHandler>(), 
		chatSession,
		100);

	ASSERT_CRASH(chatClient->Start());

	for (int i = 0; i < numberOfProcessor; ++i)
	{
		GThreadManager->Launch([=]() {
			while (true)
			{
				chatClient->GetIOCPHandler()->CallGQCS();
			}
		});
	}
	
	CS_CHAT_REQUEST packet;
	::memcpy(packet.message, "HelloWorld", 11);
	packet.size = 17;

	while (true)
	{
		this_thread::sleep_for(1s);
		shared_ptr<SendBuffer> sendBuf = make_shared<SendBuffer>(packet.size);
		sendBuf->CopyData((char*)&packet, packet.size);
		GClientSessionManager->Broadcast(sendBuf);
	}

	GThreadManager->Join();
	return 0;
}