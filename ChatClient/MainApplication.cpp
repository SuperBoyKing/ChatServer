#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"

#define	EXPORT	__declspec(dllexport)

function<shared_ptr<ServerSession>(void)> serverSession = make_shared<ServerSession>;

shared_ptr<ChatClient> chatClient = make_shared<ChatClient>(
	make_shared<ServerAddress>(),
	make_shared<IOCPHandler>(),
	serverSession);

ServerSession* session;

bool GetPacket(void* packetData, int size)
{
	mutex m;
	lock_guard<mutex> lock(m);
	if (!GRecvPacketQueue.empty())
	{
		char* packet;
		packet = reinterpret_cast<char*>(&GRecvPacketQueue.front()[0]);
		GRecvPacketQueue.pop();
		::memcpy(packetData, packet, size);

		return true;
	}

	return false;
}

extern "C"
{
	EXPORT void ChatClientStart(WCHAR* ip, __int16 port)
	{
		chatClient->Connect(ip, port);

		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		int numberOfProcessor = sysinfo.dwNumberOfProcessors;

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

		session = static_cast<ServerSession*>(chatClient->GetChatSession());
	}

	EXPORT void SendLoginPacket(char* id, const int idSize, char* pwd, const int pwdSize)
	{
		chatClient->SendLogin(id, idSize, pwd, pwdSize);
	}

	EXPORT void SendChatPacket(char* str, int size)
	{
		chatClient->SendChat(str, size);
	}

	EXPORT void	SendRoomOpenPacket(char* title, int titleSize, int userCount)
	{
		chatClient->SendRoomOpen(title, titleSize, userCount);
	}

	EXPORT void SendRoomEnterPacket(int number)
	{
		chatClient->SendRoomEnter(number);
	}

	EXPORT bool	GetPacketHeader(PACKET_HEADER* packetHeader)
	{
		if (!GRecvPacketQueue.empty())
		{
			char* packet = reinterpret_cast<char*>(&GRecvPacketQueue.front()[0]);
			::memcpy(packetHeader, packet, PACKET_HEADER_SIZE);
			return true;
		}
		return false;
	}

	EXPORT bool GetLoginPacket(SC_LOGIN_RESPONSE* packetData, int size)
	{
		return GetPacket(reinterpret_cast<void*>(packetData), size);
	}

	EXPORT bool GetChatPacket(SC_CHAT_RESPONSE* packetData, int size)
	{
		return GetPacket(reinterpret_cast<void*>(packetData), size);
	}
	
	EXPORT bool GetChatNotifyPacket(SC_CHAT_NOTIFY* packetData, int size)
	{
		return GetPacket(reinterpret_cast<void*>(packetData), size);
	}

	EXPORT bool GetRoomOpenPacket(SC_ROOM_OPEN_RESPONSE* packetData, int size)
	{
		return GetPacket(reinterpret_cast<void*>(packetData), size);
	}

	EXPORT bool GetRoomEnterPacket(SC_ROOM_ENTER_RESPONSE* packetData, int size)
	{
		return GetPacket(reinterpret_cast<void*>(packetData), size);
	}
}