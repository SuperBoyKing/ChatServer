#include "pch.h"
#include "ThreadManager.h"

ThreadManager::ThreadManager()
{
	SetTLS();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(std::function<void(int)> threadFunction, int args)
{
	std::lock_guard<mutex> lock(m_mutex);

	m_vectorThreads.push_back(thread([=]()
		{
			SetTLS();
			threadFunction(args);
		}));
}

void ThreadManager::Join()
{
	for (thread& t : m_vectorThreads)
	{
		if (t.joinable())
			t.join();
	}

	m_vectorThreads.clear();
}

void ThreadManager::SetTLS()
{
	static std::atomic<unsigned int> ThreadId = 1;
	ThreadLocalID = ThreadId.fetch_add(1);
}
