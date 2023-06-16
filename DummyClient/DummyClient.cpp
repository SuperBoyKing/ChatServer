#include "pch.h"
#include "DummySession.h"
#include <ObjBase.h>

mutex m;

template <typename PacketType>
bool GetNumberOfPacket(PacketType** packetData, int packetCount)
{
	int packetSize = sizeof(PacketType) * packetCount;

	if (packetSize <= 0)
		*packetData = nullptr;

	lock_guard<mutex> lock(m);
	if (!GRecvPacketQueue.empty())
	{
		char* packet = &GRecvPacketQueue.front()[0];
		if (*packetData != nullptr)
		{
			::memcpy(*packetData, packet + PACKET_HEADER_SIZE, packetSize);
		}
		GRecvPacketQueue.pop();

		return true;
	}

	return false;
}

bool GetPacketHeader(PACKET_HEADER* packetHeader)
{
	lock_guard<mutex> lock(m);
	if (!GRecvPacketQueue.empty())
	{
		char* packet = &GRecvPacketQueue.front()[0];
		::memcpy(packetHeader, packet, PACKET_HEADER_SIZE);
		return true;
	}
	return false;
}

bool GetPacket(void* packetData, int size)
{
	lock_guard<mutex> lock(m);
	if (!GRecvPacketQueue.empty())
	{
		char* packet = &GRecvPacketQueue.front()[0];
		::memcpy(packetData, packet + PACKET_HEADER_SIZE, size - PACKET_HEADER_SIZE);
		GRecvPacketQueue.pop();

		return true;
	}

	return false;
}

int main()
{
	this_thread::sleep_for(500ms);

	int sessionCount = 250;
	function<shared_ptr<DummySession>(void)> serverSession = make_shared<DummySession>;
	shared_ptr<ChatClient> chatClient = make_shared<ChatClient>(
		make_shared<ServerAddress>(),
		make_shared<IOCPHandler>(),
		serverSession,
		sessionCount);

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

	string idTemplate = "tester";

	cout << "Start send Register & Login Reqeust..." << endl;
	this_thread::sleep_for(2s);
	// DummyClients test for Register & Login
	for (int i = 0; i < sessionCount; ++i)
	{
		int num = i + 1;
		string&& id = idTemplate + to_string(num);
		chatClient->SendRegister(id.c_str(), id.size() + 1, "1234", 5, i);
		chatClient->SendLogin(id.c_str(), id.size() + 1, "1234", 5, i);
		this_thread::sleep_for(1ms);
	}


	cout << "\nStart send Open Room Request..." << endl;
	this_thread::sleep_for(2s);
	// Open Room for DummyTest.
	chatClient->SendRoomOpen("DummyTesterRoom\0", 16, sessionCount);


	cout << "\nStart send Enter Room Request..." << endl;
	this_thread::sleep_for(2s);
	// ENTER TEST
	for (int i = 0; i < sessionCount; ++i)
	{
		chatClient->SendRoomEnter(1, i);
	}
	
	cout << "\nStart send Chat Request..." << endl;
	this_thread::sleep_for(2s);
	// CHAT TEST
	string chatTestTemplate = "Hello, I'm Tester";
	for (int i = 0; i < sessionCount; ++i)
	{
		int num = i + 1;
		string&& chatTestStr = chatTestTemplate + to_string(num);
		chatClient->SendChat(chatTestStr.c_str(), chatTestStr.size() + 1, i);
	}

	cout << "\nStart send Leave Room Request..." << endl;
	this_thread::sleep_for(2s);
	// LEAVE TEST
	for (int i = 0; i < sessionCount; ++i)
	{
		chatClient->SendRoomLeave(1, i);
	}

	//cout << "\nStart Disconnect..." << endl;
	//this_thread::sleep_for(2s);
	// DisConnect TEST
	//chatClient->Disconnect(GNumberOfProcessor);

	cout << "test Complete : " <<  sessionCount << endl;
	return 0;
}