#include "pch.h"
#include "ServerSession.h"
#include "ServerPacketHandler.h"
#include <ObjBase.h>

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
		::memcpy(packetData, packet + sizeof(PACKET_HEADER), size - PACKET_HEADER_SIZE);
		GRecvPacketQueue.pop();

		return true;
	}

	return false;
}

template <typename PacketType>
bool GetNumberOfPacket(PacketType** packetData, int packetCount)
{
	int packetSize = sizeof(PacketType) * packetCount;
	PacketType* newArray = reinterpret_cast<PacketType*>(CoTaskMemAlloc(packetSize));

	if (packetSize == 0)
		newArray = nullptr;

	mutex m;
	{
		lock_guard<mutex> lock(m);
		if (!GRecvPacketQueue.empty())
		{
			char* packet = reinterpret_cast<char*>(&GRecvPacketQueue.front()[0]);
			if (newArray != nullptr)
			{
				::memcpy(newArray, packet + PACKET_HEADER_SIZE, packetSize);
			}
			GRecvPacketQueue.pop();

			*packetData = newArray;
			return true;
		}
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

	EXPORT void SendConnectPacket()
	{
		chatClient->SendConnect();
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

	EXPORT bool GetConnectPacket(SC_CONNECT_RESPONSE** packetData, int size)
	{
		return GetNumberOfPacket(packetData, size);
	}

	EXPORT bool GetLoginPacket(SC_LOGIN_RESPONSE* packetData, int size)
	{
		// chatclient userID 갱신 필요
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

	EXPORT bool GetRoomOpenNotifyPacket(SC_ROOM_OPEN_NOTIFY* packetData, int size)
	{
		return GetPacket(reinterpret_cast<void*>(packetData), size);
	}

	EXPORT bool GetRoomEnterPacket(SC_ROOM_ENTER_RESPONSE* packetData, int size)
	{
		return GetPacket(reinterpret_cast<void*>(packetData), size);
	}

	EXPORT bool GetRoomUserNotifyPacket(SC_ROOM_USERLIST_NOTIFY** packetData, int size)
	{
		return GetNumberOfPacket(packetData, size);
		//return false;
	}

	EXPORT bool GetRoomEnterUserNotify(SC_ROOM_ENTER_USER_NOTIFY* packetData, int size)
	{
		return GetPacket(reinterpret_cast<void*>(packetData), size);
	}
}