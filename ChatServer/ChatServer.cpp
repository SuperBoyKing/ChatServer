#include "pch.h"
#include "ClientSession.h"
#include "DBManager.h"

int main()
{
	GDBManager->Run("127.0.0.1", 6379);
	// Test Code
	GDBManager->insert("0311pjs", "1234");
	GDBManager->insert("Lilly", "1234");
	GDBManager->insert("Minsu", "1234");
	GDBManager->insert("kim1357", "1234");
	GDBManager->insert("Jini5858", "1234");

	function<shared_ptr<ClientSession>(void)> clientSession = make_shared<ClientSession>;

	shared_ptr<ChatServer> chatServer = make_shared<ChatServer>(
		make_shared<ServerAddress>(L"127.0.0.1", SERVER_PORT),
		make_shared<IOCPHandler>(),
		clientSession,
		100);

	GDBManager->SetIOCPHandle(chatServer->GetIOCPHandler()->GetIocpHandle());

	ASSERT_CRASH(chatServer->Start());

	for (int i = 0; i < GNumberOfProcessor; ++i)
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