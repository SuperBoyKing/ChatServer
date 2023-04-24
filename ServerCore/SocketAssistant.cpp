#include "pch.h"
#include "SocketAssistant.h"

LPFN_ACCEPTEX SocketAssistant::acceptEx;

void SocketAssistant::Init()
{
    WSAData wsaData;
    ASSERT_CRASH(::WSAStartup(MAKEWORD(2, 2), &wsaData) == 0);

    SOCKET dummySocket = INVALID_SOCKET;
    ASSERT_CRASH(GetExFunctionPointer(dummySocket, WSAID_ACCEPTEX, OUT reinterpret_cast<LPVOID*>(&acceptEx)));

    ::closesocket(dummySocket);
}

void SocketAssistant::Clear()
{
    ::WSACleanup();
}

bool SocketAssistant::GetExFunctionPointer(SOCKET socket, GUID guid, OUT LPVOID* fn)
{
    DWORD bytes;
    return ::WSAIoctl(socket, SIO_GET_EXTENSION_FUNCTION_POINTER, &guid, sizeof(guid),
            reinterpret_cast<LPVOID*>(&acceptEx), sizeof(acceptEx), &bytes, NULL, NULL);
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

void SocketAssistant::SocketClose(SOCKET socket)
{
    if (socket != INVALID_SOCKET)
        ::closesocket(socket);
    
    socket = INVALID_SOCKET;
}
