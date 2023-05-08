#include "pch.h"
#include "ThreadManager.h"
#include "ClientListener.h"

int main()
{
	shared_ptr<ClientListener> clientListener = make_shared<ClientListener>(L"127.0.0.1", SERVER_PORT);
	clientListener->SetUpListener(1);

	GThreadManager->Launch(
		[=]() {
			while (true)
			{
				GIOCPHandler->CallGQCS();
			}
		});

	cout << "Server Start!" << endl;

	const char* a = "Broadcast";

	while (true)
	{
		this_thread::sleep_for(500ms);
		shared_ptr<SendBuffer> sendBuffer = make_shared<SendBuffer>(MAX_SEND_BUFFER_SIZE);
		sendBuffer->CopyData((char*)a, sizeof(char) * 10);
		GClientSessionManager->Broadcast(sendBuffer);
	}

	GThreadManager->Join();

	return 0;
}