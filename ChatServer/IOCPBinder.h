enum class IoOperation : unsigned __int8
{
	ACCEPT,
	SEND,
	RECV,
	CONNECT,
	DISCONNECT,
};

class IOCPBinder : public OVERLAPPED
{
public:
	IOCPBinder() = delete;
	IOCPBinder(IoOperation ioOperaiton);

	void Init();

	inline IoOperation GetType() const { return m_ioOperation; }

private:
	IoOperation m_ioOperation;
};

