#pragma once

/*--------------------
	SocketAssistant
----------------------*/

class SocketAssistant
{
public:
	static LPFN_ACCEPTEX AcceptEx;

public:
	// Init & Clear
	static void Init();
	static void Clear();
	
	// Create & Set Socket Opt
	static SOCKET CreateSocket();
	static bool SetReuseAddress(SOCKET socket, bool flag = true);
	static bool SetLinger(SOCKET socket, unsigned short onoff, unsigned short linger);
	static bool SetUpdateClientSocket(SOCKET socket, SOCKET listenSocket);
	static bool SetTcpNoDelay(SOCKET socket, bool flag);
	
	// Bind/Listen & Close
	static bool SetBind(SOCKET socket, SOCKADDR_IN serverAddr);
	static bool SetListen(SOCKET socket, int backlog = SOMAXCONN);
	static bool SetBindAnyAddress(SOCKET socket, unsigned short port);
	static void SocketClose(SOCKET socket);

private:
	static bool GetExFunctionPointer(SOCKET socket, GUID guid, OUT LPVOID* fn);
};

template<typename T>
static inline bool SetSockOpt(SOCKET socket, int level, int optName, T optVal)
{
	return SOCKET_ERROR != ::setsockopt(socket, level, optName, reinterpret_cast<const char*>(&optVal), sizeof(T));
}