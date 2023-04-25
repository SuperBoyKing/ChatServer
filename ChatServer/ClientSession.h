#pragma once

class ClientSession
{
public:
	ClientSession();
	ClientSession(SOCKET clientSocket);
	~ClientSession();

	SOCKET GetSock() const { return m_socket; }
	char* GetRecvBuffer() { return m_recvBuffer; }
	int GetRecvBufferSize() const { return m_recvSize; }

	void Disconnect();

private:
	SOCKET m_socket;
	char m_recvBuffer[MAX_RECV_BUFFER];
	int m_recvSize;
};

