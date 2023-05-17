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

extern "C"
{
	EXPORT void ChatInit(WCHAR* ip, __int16 port)
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

	EXPORT void ChatLogin(char* id, char* pwd, const int idSize, const int pwdSize)
	{
		chatClient->SendLogin(id, pwd, idSize, pwdSize);
	}

	EXPORT void ChatMsg(char* str, int size)
	{
		chatClient->SendChat(str, size);
	}

	EXPORT void	GetPacketHeader(PACKET_HEADER* packetHeader)
	{
		if (!GRecvPacketQueue.empty())
		{
			char* packet = reinterpret_cast<char*>(&GRecvPacketQueue.front()[0]);
			::memcpy(packetHeader, packet, PACKET_HEADER_SIZE);
		}
	}
	
	EXPORT bool GetChatReqPacket(SC_CHAT_RESPONSE* packetData, int size)
	{
		if (!GRecvPacketQueue.empty())
		{
			char* packet = reinterpret_cast<char*>(&GRecvPacketQueue.front()[0]);
			::memcpy(packetData, packet, size);
			GRecvPacketQueue.pop();

			return true;
		}

		return false;
	}

	//EXPORT void GetPacketData(char* res)
	//{
	//	if (!GRecvPacketQueue.empty())
	//	{
	//		res = GRecvPacketQueue.front();
	//		GServerPacketHandler->HandlePacket(session->GetSock(), res);
	//		GRecvPacketQueue.pop();
	//	}
	//}

	//EXPORT void GetPacketData(char* res)
	//{
	//	if (!GRecvPacketQueue.empty())
	//	{
	//		res = GRecvPacketQueue.front();
	//		GServerPacketHandler->HandlePacket(session->GetSock(), res);
	//		GRecvPacketQueue.pop();
	//	}
	//}

	//EXPORT void GetPacketData(char* res)
	//{
	//	if (!GRecvPacketQueue.empty())
	//	{
	//		res = GRecvPacketQueue.front();
	//		GServerPacketHandler->HandlePacket(session->GetSock(), res);
	//		GRecvPacketQueue.pop();
	//	}
	//}
}