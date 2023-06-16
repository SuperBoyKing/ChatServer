#pragma once

#define SERVER_PORT					2023
#define MAX_CLIENT_SESSION			1000	// 최대 서버 접속 클라이언트 수
#define MAX_CLIENT_USER				250		// 최대 방 접속 클라이언트 수
#define	MAX_ROOM_COUNT				500		// 최대 방 갯 수
#define MAX_ROOM_TITLE_LENGTH		50		// 최대 방 제목 길이
#define MAX_USER_ID_LENGTH			32		// 최대 ID 길이
#define MAX_USER_PW_LENGTH			32		// 최대 PW 길이
#define MAX_CHAT_LENGTH				256		// 최대 chat 길이
#define MAX_RECV_BUFFER_SIZE		4096
#define MAX_SEND_BUFFER_SIZE		65565
#define PACKET_HEADER_SIZE		sizeof(PACKET_HEADER)
#define	EXPORT					__declspec(dllexport)

// ChatSession 내부에서 Chat Application의 공유포인터를 통한 멤버변수 접근 메크로
#define CHAT_IOCP_HANDLER		m_chatApp.lock()->GetIOCPHandler()
#define CHAT_SESSION_TYPE		m_chatApp.lock()->GetSessionType()
#define CHAT_ADDRESS			m_chatApp.lock()->GetAddress()
#define CHAT_MAX_SESSION_COUNT	m_chatApp.lock()->GetMaxSessionCount()
#define CHAT_SESSION			m_chatApp.lock()->GetSession()()