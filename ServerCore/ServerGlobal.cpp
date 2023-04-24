#include "pch.h"
#include "ServerGlobal.h"
#include "SocketAssistant.h"
#include "ThreadManager.h"

ThreadManager*	GThreadManager = nullptr;

class ServerGlobal
{
public:
	ServerGlobal()
	{
		GThreadManager = new ThreadManager();
		SocketAssistant::Init();
	}

	~ServerGlobal()
	{
		delete GThreadManager;
		SocketAssistant::Clear();
	}
} GServerGlobal;