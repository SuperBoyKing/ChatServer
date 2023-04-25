#pragma once

enum class IoType : unsigned __int8
{
	ACCEPT,
	SEND,
	RECV,
	CONNECT,
	DISCONNECT,
};

class IOCPOperation : public OVERLAPPED
{
public:
	IOCPOperation() = delete;
	IOCPOperation(IoType ioType);

	void Init();

	inline IoType GetType() const { return m_ioType; }

private:
	IoType m_ioType;
};

