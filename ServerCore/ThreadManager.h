#pragma once
class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(std::function<void(void)> threadFunction);

	void Join();

	static void SetTLS();

private:
	mutex m_mutex;
	vector<thread> m_vectorThreads;
};

