#include "pch.h"
#include "ServerSession.h"
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

extern "C"
{
	bool isLoop;
	int numberOfProcessor;
	shared_ptr<ChatClient> chatClient;
	function<shared_ptr<ServerSession>(void)> serverSession = make_shared<ServerSession>;
	
	EXPORT void ChatInit()
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		numberOfProcessor = sysinfo.dwNumberOfProcessors;

		chatClient = make_shared<ChatClient>(
			make_shared<ServerAddress>(),
			make_shared<IOCPHandler>(),
			serverSession);
	}

	EXPORT void ChatClientStart(WCHAR* ip, __int16 port)
	{
		isLoop = true;
		chatClient->SetAddress(ip, port);

		ASSERT_CRASH(chatClient->Start());

		for (int i = 0; i < numberOfProcessor; ++i)
		{
			GThreadManager->Launch([=]() {
				while (isLoop)
				{
					chatClient->GetIOCPHandler()->CallGQCS();
				}
			});
		}
	}
}

int main()
{

}