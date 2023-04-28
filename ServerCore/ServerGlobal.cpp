#include "pch.h"
#include "ServerGlobal.h"
#include "SocketAssistant.h"
#include "ThreadManager.h"
#include "IOCPHandler.h"

ThreadManager*	GThreadManager	= nullptr;
IOCPHandler*	GIOCPHandler	= nullptr;

class ServerGlobal
{
public:
	ServerGlobal()
	{
		GThreadManager = new ThreadManager();
		GIOCPHandler = new IOCPHandler();
		SocketAssistant::Init();
	}

	~ServerGlobal()
	{
		delete GThreadManager;
		delete GIOCPHandler;
		SocketAssistant::Clear();
	}
} GServerGlobal;