#include "pch.h"
#include "ThreadManager.h"
#include "ClientListener.h"

class ChatServer : public ClientSession
{
public:
	void OnRecv(char* buffer, unsigned int len) override
	{
		cout << buffer << ", Len : " << endl;
	}
};

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
	GThreadManager->Join();

	return 0;
}