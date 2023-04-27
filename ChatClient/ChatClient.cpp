#include "pch.h"
#include "ThreadManager.h"

int main()
{
	this_thread::sleep_for(1s);

	ClientSession* clientSession = new ClientSession();
	clientSession->Connect();
	clientSession->RegisterSend();

	GThreadManager->Launch([=]() {
		while (true)
		{
			clientSession->RegisterSend();
		}
	});

	GThreadManager->Join();
	delete clientSession;
	return 0;
}