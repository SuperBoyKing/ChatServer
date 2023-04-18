#include "pch.h"
#include "ServerGlobal.h"
#include "SocketAssistant.h"

class ServerGlobal
{
public:
	ServerGlobal()
	{
		SocketAssistant::Init();
	}

	~ServerGlobal()
	{
		SocketAssistant::Clear();
	}
} GServerGlobal;