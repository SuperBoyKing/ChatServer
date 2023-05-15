#include "pch.h"
#include "ServerSession.h"

function<shared_ptr<ServerSession>(void)> serverSession = make_shared<ServerSession>;

shared_ptr<ChatClient> chatClient = make_shared<ChatClient>(
	make_shared<ServerAddress>(),
	make_shared<IOCPHandler>(),
	serverSession);

extern "C" __declspec(dllexport) void ChatInit(WCHAR * ip, __int16 port)
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
}

extern "C" __declspec(dllexport) void ChatLogin(char* id, char* pwd, const int idSize, const int pwdSize)
{
	chatClient->SendLogin(id, pwd, idSize, pwdSize);
}

extern "C" __declspec(dllexport) void ChatMsg(char* str, int size)
{
	chatClient->SendChat(str, size);
}