#include "pch.h"
#include "ServerGlobal.h"
#include "SocketAssistant.h"
#include "ThreadManager.h"
#include "IOCPHandler.h"

ThreadManager*	GThreadManager	= nullptr;
int				GNumberOfProcessor = 0;

class ServerGlobal
{
public:
	ServerGlobal()
	{
		SYSTEM_INFO sysinfo;
		GetSystemInfo(&sysinfo);
		GNumberOfProcessor = sysinfo.dwNumberOfProcessors;

		GThreadManager = new ThreadManager();
		SocketAssistant::Init();
	}

	~ServerGlobal()
	{
		delete GThreadManager;
		SocketAssistant::Clear();
	}
} GServerGlobal;