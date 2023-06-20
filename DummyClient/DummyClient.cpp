#include "pch.h"
#include "DummySession.h"
#include <ObjBase.h>

string idTemplate = "tester";
string roomTemplate = "DummyTesterRoom";
int sessionCount = 500;
function<shared_ptr<DummySession>(void)> serverSession = make_shared<DummySession>;
shared_ptr<ChatClient> chatClient = make_shared<ChatClient>(
	make_shared<ServerAddress>(),
	make_shared<IOCPHandler>(),
	serverSession,
	sessionCount);

void Test(int start, int end, int sessionCount)
{
	int roomNumber = 0;

	cout << "Start send Register & Login Reqeust..." << endl;
	this_thread::sleep_for(2s);
	// DummyClients test for Register & Login
	for (int i = start; i < end; ++i)
	{
		int num = i + 1;
		string&& id = idTemplate + to_string(num);
		chatClient->SendRegister(id.c_str(), id.size() + 1, "1234", 5, i);
		chatClient->SendLogin(id.c_str(), id.size() + 1, "1234", 5, i);
		this_thread::sleep_for(1ms);
	}

	// Open Room for DummyTest.
	cout << "\nStart send Open Room Request..." << endl;
	this_thread::sleep_for(2s);
	int num = end / 100;
	string&& roomTitle = roomTemplate + to_string(num);
	while (!chatClient->SendRoomOpen(roomTitle.c_str(), roomTitle.size() + 1, sessionCount, num)) {}

	// ENTER TEST
	cout << "\nStart send Enter Room Request..." << endl;
	this_thread::sleep_for(2s);

	{
		while (1)
		{
			lock_guard<mutex> lock(queueMutex);
			if (!GRecvRoomNumberQueue.empty())
			{
				roomNumber = GRecvRoomNumberQueue.front();
				GRecvRoomNumberQueue.pop();
				break;
			}
			this_thread::sleep_for(1ms);
		}
	}

	for (int i = start; i < end; ++i)
	{
		chatClient->SendRoomEnter(roomNumber, i);
		this_thread::sleep_for(100ms);
	}

	// CHAT TEST
	cout << "\nStart send Chat Request..." << endl;
	this_thread::sleep_for(2s);
	string chatTestTemplate = "Hello, I'm Tester";
	for (int i = start; i < end; ++i)
	{
		int num = i + 1;
		string&& chatTestStr = chatTestTemplate + to_string(num);
		chatClient->SendChat(chatTestStr.c_str(), chatTestStr.size() + 1, i);
		this_thread::sleep_for(100ms);
	}

	// LEAVE TEST
	cout << "\nStart send Leave Room Request..." << endl;
	this_thread::sleep_for(2s);
	for (int i = start; i < end; ++i)
	{
		chatClient->SendRoomLeave(roomNumber, i);
		this_thread::sleep_for(100ms);
	}

	cout << "End Test!" << endl;
}

int main()
{
	this_thread::sleep_for(3s);

	chatClient->SetAddress(L"127.0.0.1", SERVER_PORT);

	ASSERT_CRASH(chatClient->Start());

	for (int i = 0; i < GNumberOfProcessor; ++i)
	{
		GThreadManager->Launch([=]() {
			while (true)
			{
				chatClient->GetIOCPHandler()->CallGQCS();
			}
		});
	}

	int count = 5;
	for (int i = 0; i < count; ++i)
	{
		int start = i * 100;
		int end = i * 100 + 100;
		int roomNumber = i + 1;
		GThreadManager->Launch([=]() {
			Test(start, end, 100);
		});
	}
	

	cout << "test Complete : " <<  sessionCount << endl;

	GThreadManager->Join();

	return 0;
}