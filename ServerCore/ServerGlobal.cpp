#include "pch.h"
#include "ServerGlobal.h"
#include "SocketAssistant.h"
#include "ThreadManager.h"
#include "IOCPServer.h"

ThreadManager*	GThreadManager	= nullptr;
IOCPServer*		GIOCPServer		= nullptr;

class ServerGlobal
{
public:
	ServerGlobal()
	{
		GThreadManager = new ThreadManager();
		GIOCPServer = new IOCPServer();
		SocketAssistant::Init();
	}

	~ServerGlobal()
	{
		delete GThreadManager;
		delete GIOCPServer;
		SocketAssistant::Clear();
	}
} GServerGlobal;