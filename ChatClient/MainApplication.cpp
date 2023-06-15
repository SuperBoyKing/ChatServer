#include "pch.h"
#include "ServerSession.h"

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
	function<shared_ptr<ServerSession>(void)> serverSession = make_shared<ServerSession>;
	shared_ptr<ChatClient> chatClient = make_shared<ChatClient>(
		make_shared<ServerAddress>(),
		make_shared<IOCPHandler>(),
		serverSession);

	EXPORT void ChatClientStart(WCHAR* ip, __int16 port)
	{
		isLoop = true;
		chatClient->SetAddress(ip, port);

		ASSERT_CRASH(chatClient->Start());
		
		for (int i = 0; i < GNumberOfProcessor; ++i)
		{
			GThreadManager->Launch([=]() {
				while (isLoop)
				{
					chatClient->GetIOCPHandler()->CallGQCS();
				}
			});
		}
	}
	
	EXPORT void Disconnect()
	{
		isLoop = false;
		chatClient->Disconnect(GNumberOfProcessor);
		GThreadManager->Join();
	}

	EXPORT bool SendConnect()
	{
		return chatClient->SendConnect();
	}

	EXPORT void SendRoomListPacket()
	{
		chatClient->SendRoomList();
	}

	EXPORT void SendRegisterAccount(char* id, const int idSize, char* pwd, const int pwdSize)
	{
		chatClient->SendRegister(id, idSize, pwd, pwdSize);
	}

	EXPORT void SendLoginPacket(char* id, const int idSize, char* pwd, const int pwdSize)
	{
		chatClient->SendLogin(id, idSize, pwd, pwdSize);
	}

	EXPORT void SendLogoutPacket(char* id, const int idSize)
	{
		chatClient->SendLogout(id, idSize);
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

	EXPORT void SendRoomLeavePacket(int number)
	{
		chatClient->SendRoomLeave(number);
	}

	EXPORT bool	GetPacketHeader(PACKET_HEADER* packetHeader)
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

	EXPORT bool GetPacket(void* packetData, int size)
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

	EXPORT bool GetRoomListPacket(SC_ROOM_LIST_MULTIPLE** packetData, int count)
	{
		return GetNumberOfPacket(packetData, count);
	}

	EXPORT bool GetRoomUserNotifyPacket(SC_USER_LIST_NOTIFY_MULTIPLE** packetData, int count)
	{
		return GetNumberOfPacket(packetData, count);
	}
}