#pragma once

#define SERVER_PORT				2023
#define MAX_CLIENT_SESSION		1000	// �ִ� ���� ���� Ŭ���̾�Ʈ ��
#define MAX_CLIENT_USER			250		// �ִ� �� ���� Ŭ���̾�Ʈ ��
#define	MAX_ROOM_COUNT			500		// �ִ� �� �� ��
#define MAX_RECV_BUFFER_SIZE	4096
#define MAX_SEND_BUFFER_SIZE	4096
#define PACKET_HEADER_SIZE		6

// Chat Application�� ���������͸� ���� ������� ���� ��ũ��
#define CHAT_IOCP_HANDLER		m_chatApp.lock()->GetIOCPHandler()
#define CHAT_APPLICATION_TYPE	m_chatApp.lock()->GetApplicationType()
#define CHAT_ADDRESS			m_chatApp.lock()->GetAddress()
#define CHAT_MAX_SESSION_COUNT	m_chatApp.lock()->GetMaxSessionCount()
#define CHAT_SESSION			m_chatApp.lock()->GetSession()()