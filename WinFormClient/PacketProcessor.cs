using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.Windows.Forms;

namespace WinFormClient
{
    public partial class MainForm
    {
        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetConnectPacket([In, Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStruct, SizeParamIndex = 1)]
        ref SC_ROOM_LIST_MULTIPLE[] packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetPacketHeader(ref PACKET_HEADER packetHeader);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetLoginPacket(ref SC_LOGIN_RESPONSE packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetChatPacket(ref SC_CHAT_RESPONSE packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetChatNotifyPacket(ref SC_CHAT_NOTIFY packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomOpenPacket(ref SC_ROOM_OPEN_RESPONSE packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomOpenNotifyPacket(ref SC_ROOM_OPEN_NOTIFY_MULTIPLE packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomEnterPacket(ref SC_ROOM_ENTER_RESPONSE packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomEnterUserNotify(ref SC_ROOM_ENTER_USER_NOTIFY packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomUserNotifyPacket([In, Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr, SizeParamIndex = 1)]
            ref SC_USER_LIST_NOTIFY_MULTIPLE[] packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomLeavePacket(ref SC_ROOM_LEAVE_RESPONSE packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomLeaveNotifyPacket(ref SC_ROOM_LEAVE_USER_NOTIFY packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetClosePacket(ref SC_ROOM_CLOSE packetData, int size);

        Dictionary<PacketID, Action<PACKET_HEADER>> PacketFuncDictionary = new Dictionary<PacketID, Action<PACKET_HEADER>>();

        void InitProcessPacket()
        {
            PacketFuncDictionary.Add(PacketID.CONNECT_RESPONSE, ProcessConnectResponse);
            PacketFuncDictionary.Add(PacketID.LOGIN_RESPONSE, ProcessLoginResponse);
            PacketFuncDictionary.Add(PacketID.CHAT_RESPONSE, ProcessChatResponse);
            PacketFuncDictionary.Add(PacketID.CHAT_NOTIFY, ProcessChatNotify);
            PacketFuncDictionary.Add(PacketID.ROOM_OPEN_RESPONSE, ProcessRoomOpenResponse);
            PacketFuncDictionary.Add(PacketID.ROOM_OPEN_NOTIFY, ProcessRoomOpenNotify);
            PacketFuncDictionary.Add(PacketID.ROOM_ENTER_RESPONSE, ProcessRoomEnterResponse);
            PacketFuncDictionary.Add(PacketID.ROOM_ENTER_USER_NOTIFY, ProcessRoomEnterUserNotify);
            PacketFuncDictionary.Add(PacketID.ROOM_USER_LIST_NOTIFY, ProcessRoomUserListNotify);
            PacketFuncDictionary.Add(PacketID.ROOM_LEAVE_RESPONSE, ProcessRoomLeaveResponse);
            PacketFuncDictionary.Add(PacketID.ROOM_LEAVE_USER_NOTIFY, ProcessRoomLeaveNotify);
            PacketFuncDictionary.Add(PacketID.ROOM_CLOSE, ProcessRoomClose);
        }

        void ProcessPacket()
        {
            PACKET_HEADER packetHeader;
            packetHeader.size = 0;
            packetHeader.packetCount = 0;
            packetHeader.id = PacketID.LOGIN_REQUEST;

            if (GetPacketHeader(ref packetHeader))
            {
                if (PacketFuncDictionary.ContainsKey(packetHeader.id))
                {
                    PacketFuncDictionary[packetHeader.id](packetHeader);
                }
                else
                { 
                    // unknown packet ID
                }
            }
        }

        void ProcessConnectResponse(PACKET_HEADER packetHeader)
        {
            SC_ROOM_LIST_MULTIPLE[] conResPacket = new SC_ROOM_LIST_MULTIPLE[packetHeader.packetCount];

            if (GetConnectPacket(ref conResPacket, packetHeader.packetCount))
            {
                for (int i = 0; i < packetHeader.packetCount; ++i)
                {
                    roomManager.roomDictionary.Add(conResPacket[i].roomInfo.title, conResPacket[i].roomInfo);
                    this.Invoke(new Action(() =>
                    {
                        listBox_room.Items.Add(conResPacket[i].roomInfo.title);
                    }));
                }
            }
        }

        void ProcessLoginResponse(PACKET_HEADER packetHeader)
        {
            SC_LOGIN_RESPONSE loginResPacket;
            loginResPacket.result = false;

            if (GetLoginPacket(ref loginResPacket, packetHeader.size))
            {
                if (loginResPacket.result)
                {
                    this.Invoke(new Action(() =>
                    {
                        button_login.Text = "Logout";
                        textBox_ID.Enabled = false;
                        textBox_password.Enabled = false;
                        button_RoomCreate.Enabled = true;
                        button_RoomEnter.Enabled = true;
                    }));

                    IsActivatedLogin = true;
                    userID = textBox_ID.Text;
                }
                else
                {
                    MessageBox.Show("Login Failed.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        void ProcessChatResponse(PACKET_HEADER packetHeader) 
        {
            SC_CHAT_RESPONSE chatResPacket;
            chatResPacket.result = false;

            if (GetChatPacket(ref chatResPacket, packetHeader.size))
            {
                this.Invoke(new Action(() =>
                {
                    if (chatResPacket.result)
                    {
                        CheckChatLimitCount();
                        listBox_chat.Items.Add(textBox_chat.Text);
                        chatDrawIndexFlags[listBox_chat.Items.Count - 1] = ChatDrawingType.RESPONSE_CHAT;
                        textBox_chat.Clear();
                    }
                }));
            }

            this.Invoke(new Action(() =>
            {
                button_chat.Enabled = true;
            }));
        }

        void ProcessChatNotify(PACKET_HEADER packetHeader) 
        {
            SC_CHAT_NOTIFY chatNotifyPacket;
            chatNotifyPacket.userID = null;
            chatNotifyPacket.message = null;

            if (GetChatNotifyPacket(ref chatNotifyPacket, packetHeader.size))
            {
                this.Invoke(new Action(() =>
                {
                    CheckChatLimitCount();
                    listBox_chat.Items.Add(chatNotifyPacket.userID + ": ");
                    chatDrawIndexFlags[listBox_chat.Items.Count - 1] = ChatDrawingType.NOTIFY_ID;
                    listBox_chat.Items.Add(chatNotifyPacket.message);
                    chatDrawIndexFlags[listBox_chat.Items.Count - 1] = ChatDrawingType.NOTIFY_CHAT;
                }));
            }
        }

        void ProcessRoomOpenResponse(PACKET_HEADER packetHeader)
        {
            SC_ROOM_OPEN_RESPONSE roomOpenPacket;
            roomOpenPacket.roomNumber = 0;
            roomOpenPacket.result = false;

            if (GetRoomOpenPacket(ref roomOpenPacket, packetHeader.size))
            {
                this.Invoke(new Action(() =>
                {
                    listBox_room.Items.Add(roomTitle);
                    textBox_roomTitle.Text = roomTitle;
                }));

                Room room = new Room(roomOpenPacket.roomNumber, roomTitle, roomUserCount, 0);
                roomManager.roomDictionary.Add(roomTitle, room);
            }

            this.Invoke(new Action(() =>
            {
                button_RoomCreate.Enabled = true;
            }));
        }

        void ProcessRoomOpenNotify(PACKET_HEADER packetHeader)
        {
            SC_ROOM_OPEN_NOTIFY_MULTIPLE roomNotify;
            roomNotify.roomInfo.number = 0;
            roomNotify.roomInfo.title = null;
            roomNotify.roomInfo.maxUserCount = 0;
            roomNotify.roomInfo.currentUserCount = 0;

            if (GetRoomOpenNotifyPacket(ref roomNotify, packetHeader.size))
            {
                this.Invoke(new Action(() =>
                {
                    listBox_room.Items.Add(roomNotify.roomInfo.title);
                    textBox_roomTitle.Text = roomNotify.roomInfo.title;
                }));

                Room room = new Room(roomNotify.roomInfo.number, roomNotify.roomInfo.title, roomNotify.roomInfo.maxUserCount, roomNotify.roomInfo.currentUserCount);
                roomManager.roomDictionary.Add(roomNotify.roomInfo.title, room);
            }
        }

        void ProcessRoomEnterResponse(PACKET_HEADER packetHeader)
        {
            SC_ROOM_ENTER_RESPONSE roomEnterPacket;
            roomEnterPacket.result = false;

            if (GetRoomEnterPacket(ref roomEnterPacket, packetHeader.size))
            {
                if (roomEnterPacket.result)
                {
                    this.Invoke(new Action(() =>
                    {
                        // Chat UI 활성화
                        button_chat.Enabled = true;
                        textBox_chat.Enabled = true;
                        listBox_chat.Enabled = true;
                        listBox_user.Enabled = true;
                        listBox_user.Items.Add(userID);

                        // Leave 버튼 활성화
                        button_RoomLeave.Enabled = true;
                    }));

                    UserCountIncrease();
                }
                else
                {
                    textBox_roomTitle.Text = "";
                    MessageBox.Show("Chat Room is full.", "Info", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
            }
            
        }

        void ProcessRoomEnterUserNotify(PACKET_HEADER packetHeader)
        {
            SC_ROOM_ENTER_USER_NOTIFY roomEnterUserPacket;
            roomEnterUserPacket.userID = null;

            if (GetRoomEnterUserNotify(ref roomEnterUserPacket, packetHeader.size))
            {
                for (int i = 0; i < packetHeader.packetCount; ++i)
                {
                    this.Invoke(new Action(() =>
                    {
                        listBox_user.Items.Add(roomEnterUserPacket.userID);
                    }));

                    UserCountIncrease();
                }
            }
        }

        void ProcessRoomUserListNotify(PACKET_HEADER packetHeader)
        {
            SC_USER_LIST_NOTIFY_MULTIPLE[] roomUserListPacket = new SC_USER_LIST_NOTIFY_MULTIPLE[packetHeader.packetCount];

            if (GetRoomUserNotifyPacket(ref roomUserListPacket, packetHeader.packetCount))
            {
                for (int i = 0; i < packetHeader.packetCount; ++i)
                {
                    this.Invoke(new Action(() =>
                    {
                        listBox_user.Items.Add(roomUserListPacket[i].userID);
                    }));
                }
            }
        }

        void ProcessRoomLeaveResponse(PACKET_HEADER packetHeader)
        {
            SC_ROOM_LEAVE_RESPONSE roomLeavePacket;
            roomLeavePacket.result = false;

            if (GetRoomLeavePacket(ref roomLeavePacket, packetHeader.size))
            {
                if (roomLeavePacket.result)
                {
                    for (int i = 0; i < listBox_chat.Items.Count; ++i)
                        chatDrawIndexFlags[i] = ChatDrawingType.NONE;

                    this.Invoke(new Action(() =>
                    {
                        listBox_chat.Items.Clear();
                        listBox_user.Items.Clear();

                        // 만약 남은 유저가 자기 자신밖에 없다면 List에서 room 제거
                        if (Int16.Parse(textBox_userCount.Text) == 1)
                        {
                            listBox_room.Items.Remove(textBox_roomTitle.Text);
                            roomManager.roomDictionary.Remove(textBox_roomTitle.Text);
                        }

                        textBox_roomTitle.Clear();
                        textBox_userCount.Clear();

                        button_RoomEnter.Enabled = true;
                    }));
                }
                else
                {
                    this.Invoke(new Action(() =>
                    {
                        button_RoomLeave.Enabled = true;
                    }));
                }
            }
        }

        void ProcessRoomLeaveNotify(PACKET_HEADER packetHeader)
        {
            SC_ROOM_LEAVE_USER_NOTIFY roomLeaveNotify;
            roomLeaveNotify.userID = null;
            roomLeaveNotify.roomTitle = null;

            if (GetRoomLeaveNotifyPacket(ref roomLeaveNotify, packetHeader.size))
            {
                this.Invoke(new Action(() =>
                {
                    if (listBox_user.FindString(roomLeaveNotify.userID) != ListBox.NoMatches)
                    {
                        listBox_user.Items.Remove(roomLeaveNotify.userID);
                    }
                }));
            }
        }

        void ProcessRoomClose(PACKET_HEADER packetHeader)
        {
            SC_ROOM_CLOSE closePacket;
            closePacket.roomTitle = null;

            if (GetClosePacket(ref closePacket, packetHeader.size))
            {
                // 해당 Room List에서 제거
                Room room;
                if (roomManager.roomDictionary.TryGetValue(closePacket.roomTitle, out room))
                {
                    listBox_room.Items.Remove(closePacket.roomTitle);
                    roomManager.roomDictionary.Remove(closePacket.roomTitle);
                }
            }
        }

        private void UserCountIncrease()
        {
            // 수정 필요
            Room room;
            if(roomManager.roomDictionary.TryGetValue(textBox_roomTitle.Text, out room))
            {
                ++room.currentUserCount;
                roomManager.roomDictionary[textBox_roomTitle.Text] = room;

                this.Invoke(new Action(() =>
                {
                    textBox_userCount.Text = room.currentUserCount.ToString();
                }));
            }
        }

        private void CheckChatLimitCount()
        {
            if (listBox_chat.Items.Count > 512)
            {
                listBox_chat.Items.Clear();
                for (int i = 0; i < 512; ++i)
                    chatDrawIndexFlags[i] = ChatDrawingType.NONE;
            }
        }
    }
}
