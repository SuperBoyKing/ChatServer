#include "pch.h"
#include "IOCPHandler.h"
#include "ClientListener.h"
#include "ClientSessionManager.h"

ClientListener::ClientListener()
	: m_ListenSocket(INVALID_SOCKET)
{
}

ClientListener::ClientListener(const WCHAR* ip, const unsigned __int16 port)
	: m_ListenSocket(INVALID_SOCKET)
	, m_serverAddress(ip, port)
{
}

ClientListener::~ClientListener()
{
	SocketAssistant::SocketClose(m_ListenSocket);
}

void ClientListener::SetUpListener()
{
	m_ListenSocket = SocketAssistant::CreateSocket();
	if (m_ListenSocket == INVALID_SOCKET)
		return;

	if (SocketAssistant::SetLinger(m_ListenSocket, 0, 0) == false)
	{
		PRINT_WSA_ERROR("Linger Set Error");
		return;
	}

	if (SocketAssistant::SetReuseAddress(m_ListenSocket) == false)
	{
		PRINT_WSA_ERROR("ReuseAddress Set Error");
		return;
	}

	if (SocketAssistant::SetTcpNoDelay(m_ListenSocket, true) == false)
	{
		PRINT_WSA_ERROR("TcpNoDelay Set Error");
		return;
	}

	if (SocketAssistant::SetBind(m_ListenSocket, m_serverAddress.GetSockAddrIn()) == false)
	{
		PRINT_WSA_ERROR("Bind Error");
		return;
	}

	if (SocketAssistant::SetListen(m_ListenSocket) == false)
	{
		PRINT_WSA_ERROR("Listen Error");
		return;
	}

	GIOCPHandler->BindIOCompletionPort(shared_from_this());
}

void ClientListener::ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes)
{
	ASSERT_CRASH(iocpOperation->GetType() == OperationType::ACCEPT);
	cout << "Accept Complete" << endl;
	RegisterAccept();
}

void ClientListener::RegisterAccept()
{
	m_acceptOperation.Init();
	m_acceptOperation.SetType(OperationType::ACCEPT);
	m_acceptOperation.SetOwner(shared_from_this());

	shared_ptr<ClientSession> clientSession = make_shared<ClientSession>();
	SocketAssistant::SetUpdateClientSocket(clientSession->GetSock(), m_ListenSocket);
	shared_ptr<IIOCPBinder> iocpBinder = clientSession;
	GIOCPHandler->BindIOCompletionPort(iocpBinder);

	DWORD bytesReceived = 0;
	if (false == SocketAssistant::AcceptEx(m_ListenSocket, clientSession->GetSock(), clientSession->GetRecvBuffer(), 0,
		sizeof(SOCKADDR) + 16, sizeof(SOCKADDR) + 16, &bytesReceived, static_cast<LPOVERLAPPED>(&m_acceptOperation)))
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Accept error");
		}
	}

	GClientSessionManager->Add(clientSession);
}
