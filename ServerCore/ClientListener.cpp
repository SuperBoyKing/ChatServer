#include "pch.h"
#include "IOCPHandler.h"
#include "ClientListener.h"
#include "ClientSessionManager.h"

ClientListener::ClientListener()
	: m_listenSocket(SocketAssistant::CreateSocket())
	, m_chatServer(nullptr)
{
}

ClientListener::~ClientListener()
{
	SocketAssistant::SocketClose(m_listenSocket);

	for (auto acceptOperation : m_vAcceptOperations)
	{
		delete acceptOperation;
	}
	m_vAcceptOperations.clear();
}

void ClientListener::SetUpListener(shared_ptr<ChatServer> chatServer, const unsigned int maxClientSession)
{
	m_chatServer = chatServer;
	if (m_listenSocket == INVALID_SOCKET)
		return;

	if (m_chatServer->GetApplicationType() != ApplicationType::SERVER)
	{
		PRINT_ERROR("Invalid Application Type");
		return;
	}

	if (SocketAssistant::SetLinger(m_listenSocket, 0, 0) == false)
	{
		PRINT_WSA_ERROR("Linger Set Error");
		return;
	}

	if (SocketAssistant::SetReuseAddress(m_listenSocket) == false)
	{
		PRINT_WSA_ERROR("ReuseAddress Set Error");
		return;
	}

	if (SocketAssistant::SetTcpNoDelay(m_listenSocket, true) == false)
	{
		PRINT_WSA_ERROR("TcpNoDelay Set Error");
		return;
	}
	
	if (SocketAssistant::SetBind(m_listenSocket, m_chatServer->GetAddress()->GetSockAddrIn()) == false)
	{
		PRINT_WSA_ERROR("Bind Error");
		return;
	}

	if (SocketAssistant::SetListen(m_listenSocket) == false)
	{
		PRINT_WSA_ERROR("Listen Error");
		return;
	}

	for (unsigned int i = 0; i < maxClientSession; ++i)	// accept를 수행할 최대 Operation 갯 수
	{
		AcceptOperation* acceptOperation = new AcceptOperation();
		acceptOperation->SetOwner(shared_from_this());
		m_vAcceptOperations.push_back(acceptOperation);
		RegisterAccept(acceptOperation);
	}
}

void ClientListener::ProcessOperation(IOCPOperation* iocpOperation, unsigned int numberOfBytes)
{
	ASSERT_CRASH(iocpOperation->GetType() == OperationType::ACCEPT);
	AcceptOperation* acceptOperation = static_cast<AcceptOperation*>(iocpOperation);
	ProcessAccept(acceptOperation);
}

void ClientListener::ProcessAccept(AcceptOperation* acceptOperation)
{
	shared_ptr<ClientSession> clientSession = acceptOperation->GetSession();

	cout << "Accept Complete : " << clientSession->GetSock() << "(Socket Number)" << endl;

	SocketAssistant::SetUpdateClientSocket(clientSession->GetSock(), m_listenSocket);

	clientSession->ProcessConnect();

	RegisterAccept(acceptOperation);	// 다음 accept를 위해 재등록
}

void ClientListener::RegisterAccept(AcceptOperation* acceptOperation)
{
	// 클라이언트 세션 생성 및 등록
	shared_ptr<ClientSession> clientSession = make_shared<ClientSession>();

	GIOCPHandler->BindIOCompletionPort(clientSession);

	acceptOperation->Init();
	acceptOperation->SetType(OperationType::ACCEPT);
	acceptOperation->SetSession(clientSession);

	DWORD bytesReceived = 0;
	if (false == SocketAssistant::AcceptEx(m_listenSocket, clientSession->GetSock(), clientSession->m_recvBuffer.GetWritePos(), 0,
		sizeof(SOCKADDR) + 16, sizeof(SOCKADDR) + 16, &bytesReceived, static_cast<LPOVERLAPPED>(acceptOperation)))
	{
		int errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			PRINT_WSA_ERROR("Accept error");
		}
	}
}
