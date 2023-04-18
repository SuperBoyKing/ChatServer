#pragma once

/*--------------------
	SocketAssistant
----------------------*/

class SocketAssistant
{
public:
	static LPFN_ACCEPTEX acceptEx;

public:
	// Init & Clear
	static void Init();
	static void Clear();
	
	// Create & Set Socket Opt
	static SOCKET CreateSocket();
	static bool SetBind(SOCKET socket, SOCKADDR_IN serverAddr);
	static bool SetListen(SOCKET socket, int backlog = SOMAXCONN);
	static bool SetReuseAddress(SOCKET socket, bool flag = true);

	static void SocketClose(SOCKET socket);

private:
	static bool GetExFunctionPointer(SOCKET socket, GUID guid, OUT LPVOID* fn);
};