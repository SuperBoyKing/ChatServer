#include "pch.h"
#include "ThreadManager.h"
#include "ClientListener.h"

int main()
{
	shared_ptr<ClientListener> clientListener = make_shared<ClientListener>(L"127.0.0.1", SERVER_PORT);
	clientListener->SetUpListener();

	GThreadManager->Launch(
		[=]() {
			while (true)
			{ 
				GIOCPHandler->CallGQCS();
			}
		});

	clientListener->RegisterAccept();

	GThreadManager->Join();

	return 0;
}