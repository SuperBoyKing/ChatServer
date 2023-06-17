#include "pch.h"
#include "DBManager.h"

unique_ptr<DBManager> GDBManager = make_unique<DBManager>();

void DBManager::Run(string ip, UINT16 port, int threadCount)
{
	while (true)
	{
		if (m_conn.connect(ip, port))
		{
			std::cout << "Database connection success!" << endl;
			m_isEventLoop = true;
			m_isPQCSLoop = true;
			break;
		}
		else
		{
			std::cout << "Database connection error : " << m_conn.getErrorStr() << endl;
		}
		this_thread::sleep_for(1500ms); // Connection ´ë±â
	}

	for (int i = 0; i < threadCount; ++i)
	{
		m_DBThreadManager.Launch([=]() {
			while (m_isEventLoop)
			{
				ProcesssDBEvent();
				this_thread::yield();
			}
		});
	}

	for (int i = 0; i < threadCount; ++i)
	{
		m_PQCSThreadManager.Launch([=]() {
			while (m_isPQCSLoop)
			{
				CallPQCS();
			}
		});
	}
}

void DBManager::Disconnect()
{
	m_dbEventOperation.ReleaseOwner();

	m_isEventLoop = false;
	m_DBThreadManager.Join();

	m_isPQCSLoop = false;
	m_PQCSThreadManager.Join();

	m_conn.disConnect();
}

bool DBManager::insert(const char* id, const char* pw)
{
	uint32_t retval;

	if (m_conn.set(id, pw, retval))
	{
		cout << "Database insert success. ID: " << id << endl;
		return true;
	}

	return false;
}

void DBManager::ProcesssDBEvent()
{
	auto dbEvent = TakeOutRequestEvent();
	if (dbEvent.session != nullptr && dbEvent.packet.id == PacketID::LOGIN_REQUEST)
	{
		bool isGet = false;
		SC_LOGIN_RESPONSE resPacket;
		std::string value;
		{
			lock_guard<mutex> lock(m_getMutex);
			isGet = m_conn.get(dbEvent.packet.userID, value);
		}

		if (isGet)
		{
			if (value.compare(dbEvent.packet.userPW) == 0)
			{
				resPacket.result = true;
			}
		}

		DB_LOGIN_RESPONSE dbResEvent;
		dbResEvent.packet = resPacket;
		dbResEvent.session = dbEvent.session;
		PushReponseEvent(dbResEvent);
	}
}

void DBManager::PushReponseEvent(DB_LOGIN_RESPONSE& loginResponseEvent)
{
	lock_guard<mutex> lock(m_responseMutex);
	m_responseEventDeque.push_back(loginResponseEvent);
}

DB_LOGIN_REQUEST DBManager::TakeOutRequestEvent()
{
	lock_guard<mutex> lock(m_requestMutex);

	if (m_requestEventDeque.empty())
	{
		return DB_LOGIN_REQUEST();
	}

	DB_LOGIN_REQUEST reqEvent = m_requestEventDeque.front();
	m_requestEventDeque.pop_front();

	return reqEvent;
}

void DBManager::CallPQCS()
{
	auto dbResEvent = TakeOutResponseEvent();

	if (dbResEvent.session.get() != nullptr)
	{
		DBResOperation* DBOpertaion = new DBResOperation();
		DBOpertaion->Init();
		DBOpertaion->SetOwner(dbResEvent.session);
		DBOpertaion->sendBuffer = make_shared<SendBuffer>(dbResEvent.packet.size);
		DBOpertaion->sendBuffer->CopyData(reinterpret_cast<void*>(&dbResEvent.packet), dbResEvent.packet.size);

		if (false == ::PostQueuedCompletionStatus(m_iocpHandle, dbResEvent.packet.size, 0, DBOpertaion))
		{
			PRINT_WSA_ERROR("PostQueuedCompletionStatus Error");
		}
	}
}

void DBManager::PushRequestEvent(DB_LOGIN_REQUEST& loginRequestEvent)
{
	lock_guard<mutex> lock(m_requestMutex);
	m_requestEventDeque.push_back(loginRequestEvent);
}

DB_LOGIN_RESPONSE DBManager::TakeOutResponseEvent()
{
	lock_guard<mutex> lock(m_responseMutex);

	if (m_responseEventDeque.empty())
	{
		return DB_LOGIN_RESPONSE();
	}

	DB_LOGIN_RESPONSE resEvent = m_responseEventDeque.front();
	m_responseEventDeque.pop_front();

	return resEvent;
}

bool DBManager::Search(string userID)
{
	std::string value;
	lock_guard<mutex> lock(m_getMutex);
	if (m_conn.get(userID, value))
	{
		return true;
	}

	return false;
}
