#include "pch.h"
#include "IOCPServer.h"
#include "ClientListener.h"
#include "ClientSessionManager.h"

ClientListener::ClientListener()
{
	m_serverAddress = new ServerAddress();
	Init();
}

ClientListener::ClientListener(const WCHAR* ip, const unsigned __int16 port)
	:m_ListenSocket(INVALID_SOCKET)
{
	m_serverAddress = new ServerAddress(ip, port);
	Init();
}

ClientListener::~ClientListener()
{
	SocketAssistant::SocketClose(m_ListenSocket);
}

void ClientListener::Init()
{
	m_ListenSocket = SocketAssistant::CreateSocket();
	if (m_ListenSocket == INVALID_SOCKET)
		return;

	if (SocketAssistant::SetLinger(m_ListenSocket, 0, 0) == false)
	{
		PRINT_WSA_ERROR("Linger Set Error", WSAGetLastError());
		return;
	}

	if (SocketAssistant::SetReuseAddress(m_ListenSocket) == false)
	{
		PRINT_WSA_ERROR("ReuseAddress Set Error", WSAGetLastError());
		return;
	}

	if (SocketAssistant::SetTcpNoDelay(m_ListenSocket, true) == false)
	{
		PRINT_WSA_ERROR("TcpNoDelay Set Error", WSAGetLastError());
		return;
	}

	if (SocketAssistant::SetBind(m_ListenSocket, m_serverAddress->GetSockAddrIn()) == false)
	{
		PRINT_WSA_ERROR("Bind Error", WSAGetLastError());
		return;
	}

	if (SocketAssistant::SetListen(m_ListenSocket) == false)
	{
		PRINT_WSA_ERROR("Listen Error", WSAGetLastError());
		return;
	}
}

void ClientListener::ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes)
{
	ASSERT_CRASH(iocpOperation->GetType() == OperationType::ACCEPT);
	ProcessAccept();
}

void ClientListener::ProcessAccept()
{
	while (true)
	{
		SOCKADDR_IN serveraddr = m_serverAddress->GetSockAddrIn();
		int addrLen = sizeof(serveraddr);

		SOCKET clientSocket = ::accept(m_ListenSocket, (sockaddr*)(&serveraddr), &addrLen);
		if (clientSocket == INVALID_SOCKET)
		{
			PRINT_WSA_ERROR("Accept error", WSAGetLastError());
		}
		else
		{
			cout << "Accept Client" << endl;
		}

		shared_ptr<ClientSession> clientSession = make_shared<ClientSession>(clientSocket);
		GClientSessionManager->Add(clientSession);

		shared_ptr<IIOCPBinder> iocpBinder = clientSession;

		GIOCPServer->BindIOCompletionPort(*iocpBinder.get());
		clientSession->RegisterRecv();
	}
}
