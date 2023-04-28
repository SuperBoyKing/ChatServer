#include "pch.h"
#include "ThreadManager.h"
#include "ClientListener.h"

int main()
{
	shared_ptr<ClientListener> clientListener = make_shared<ClientListener>(L"127.0.0.1", SERVER_PORT);
	clientListener->SetUpListener(2);

	GThreadManager->Launch(
		[=]() {
			while (true)
			{ 
				GIOCPHandler->CallGQCS();
			}
		});

	cout << "Server Start!" << endl;
	GThreadManager->Join();

	return 0;
}