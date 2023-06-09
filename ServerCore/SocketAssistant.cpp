#include "pch.h"
#include "SocketAssistant.h"

LPFN_ACCEPTEX       SocketAssistant::AcceptEx;
LPFN_CONNECTEX	    SocketAssistant::ConnectEx;
LPFN_DISCONNECTEX	SocketAssistant::DisConnectEx;

void SocketAssistant::Init()
{
    WSAData wsaData;
    ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);

    SOCKET dummySocket = CreateSocket();
    ASSERT_CRASH(GetExFunctionPointer(dummySocket, WSAID_ACCEPTEX, reinterpret_cast<LPVOID*>(&AcceptEx)));
    ASSERT_CRASH(GetExFunctionPointer(dummySocket, WSAID_CONNECTEX, reinterpret_cast<LPVOID*>(&ConnectEx)));
    ASSERT_CRASH(GetExFunctionPointer(dummySocket, WSAID_DISCONNECTEX, reinterpret_cast<LPVOID*>(&DisConnectEx)));
    SocketClose(dummySocket);
}

void SocketAssistant::Clear()
{
    ::WSACleanup();
}

bool SocketAssistant::GetExFunctionPointer(SOCKET socket, GUID guid, LPVOID* fn)
{
    DWORD bytes;
    return SOCKET_ERROR != ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid),
        fn, sizeof(*fn), &bytes, NULL, NULL);
}

SOCKET SocketAssistant::CreateSocket()
{
    return ::WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, WSA_FLAG_OVERLAPPED);
}

bool SocketAssistant::SetBind(SOCKET socket, SOCKADDR_IN serverAddr)
{
    return SOCKET_ERROR != ::bind(socket, (sockaddr*)&serverAddr, sizeof(serverAddr));
}

bool SocketAssistant::SetListen(SOCKET socket, int backlog)
{
    return SOCKET_ERROR != ::listen(socket, backlog);
}

bool SocketAssistant::SetReuseAddress(SOCKET socket, bool flag)
{
    return SetSockOpt(socket, SOL_SOCKET, SO_REUSEADDR, flag);
}

bool SocketAssistant::SetLinger(SOCKET socket, unsigned short onoff, unsigned short linger)
{
    LINGER stLinger;
    stLinger.l_onoff = onoff;
    stLinger.l_linger = linger;

    return SetSockOpt(socket, SOL_SOCKET, SO_LINGER, stLinger);
}

bool SocketAssistant::SetUpdateClientSocket(SOCKET socket, SOCKET listenSocket)
{
    return SetSockOpt(socket, SOL_SOCKET, SO_UPDATE_ACCEPT_CONTEXT, listenSocket);
}

bool SocketAssistant::SetTcpNoDelay(SOCKET socket, bool flag)
{
    return SetSockOpt(socket, SOL_SOCKET, TCP_NODELAY, flag);
}

bool SocketAssistant::SetBindAnyAddress(SOCKET socket, unsigned short port)
{
    SOCKADDR_IN sockAddr;
    sockAddr.sin_family = AF_INET;
    sockAddr.sin_addr.s_addr = ::htonl(INADDR_ANY);
    sockAddr.sin_port = htons(port);

    return SOCKET_ERROR != ::bind(socket, (sockaddr*)(&sockAddr), sizeof(SOCKADDR_IN));
}

void SocketAssistant::SocketClose(SOCKET socket)
{
    if (socket != INVALID_SOCKET)
        ::closesocket(socket);
    
    socket = INVALID_SOCKET;
}
