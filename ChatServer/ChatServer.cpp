#include "pch.h"
#include "ClientSession.h"
#include "ClientPacketHandler.h"

int main()
{
	function<shared_ptr<ClientSession>(void)> clientSession = make_shared<ClientSession>;

	shared_ptr<ChatServer> chatServer = make_shared<ChatServer>(
		make_shared<ServerAddress>(L"127.0.0.1", SERVER_PORT),
		make_shared<IOCPHandler>(),
		clientSession,
		100);

	ASSERT_CRASH(chatServer->Start());

	SYSTEM_INFO sysinfo;
	GetSystemInfo(&sysinfo);
	int numberOfProcessor = sysinfo.dwNumberOfProcessors;

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

	GThreadManager->Join();

	return 0;
}