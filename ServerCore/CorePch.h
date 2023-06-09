#pragma once
using namespace std;

/*-----------------
	Network/IO
-------------------*/
#include <iostream>
#include <windows.h>
#include <winsock2.h>
#include <mswsock.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")

/*-----------------
	Thread/Async
-------------------*/
#include <thread>
#include <mutex>
#include <functional>

/*--------------------
	Data Structure
---------------------*/
#include <vector>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <queue>

/*--------------------
	Custom Header
---------------------*/
#include "ServerGlobal.h"
#include "ServerTLS.h"
#include "ServerMacro.h"
#include "Defines.h"
#include "ServerAddress.h"
#include "SocketAssistant.h"
#include "IOCPHandler.h"
#include "IOCPOperation.h"
#include "ClientListener.h"
#include "ChatSession.h"
#include "ClientSessionManager.h"
#include "RecvBuffer.h"
#include "SendBuffer.h"
#include "ChatApplication.h"
#include "ThreadManager.h"
#include "Packet.h"


