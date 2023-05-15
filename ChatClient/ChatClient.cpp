#include "pch.h"
#include "ServerSession.h"

int main()
{
	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numberOfProcessor = sysinfo.dwNumberOfProcessors;

	function<shared_ptr<ServerSession>(void)> serverSession = make_shared<ServerSession>;

	this_thread::sleep_for(500ms);
	shared_ptr<ChatClient> chatClient = make_shared<ChatClient>(
		make_shared<ServerAddress>(),
		make_shared<IOCPHandler>(), 
		serverSession);

	chatClient->Connect(L"127.0.0.1", SERVER_PORT);

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

	while (true)
	{
		this_thread::sleep_for(1s);
		chatClient->SendChat("HelloWorld", 11);
	}

	GThreadManager->Join();
	return 0;
}