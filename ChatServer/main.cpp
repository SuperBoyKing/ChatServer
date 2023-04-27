#include "pch.h"
#include "ThreadManager.h"
#include "ClientListener.h"

int main()
{
	ClientListener clientListener(L"127.0.0.1", SERVER_PORT);
	
	GThreadManager->Launch(
		[=]() {
			while (true)
			{ 
				GIOCPServer->CallGQCS();
			}
		});

	clientListener.ProcessAccept();

	GThreadManager->Join();

	return 0;
}