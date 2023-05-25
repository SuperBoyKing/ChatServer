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
        static extern bool GetRoomUserNotifyPacket([In, Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr, SizeParamIndex = 1)]
            ref SC_USER_LIST_NOTIFY_MULTIPLE[] packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomEnterUserNotify(ref SC_ROOM_ENTER_USER_NOTIFY packetData, int size);

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
            PacketFuncDictionary.Add(PacketID.ROOM_LEAVE_NOTIFY, ProcessRoomLeaveNotify);
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
                        CheckLimitChatCount();
                        listBox_chat.Items.Add(textBox_chat.Text);
                        chatDrawIndexFlags[listBox_chat.Items.Count - 1] = true;
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
                    CheckLimitChatCount();
                    var msg = $"{chatNotifyPacket.userID}: {chatNotifyPacket.message}";
                    listBox_chat.Items.Add(msg);
                    chatDrawIndexFlags[listBox_chat.Items.Count - 1] = false;
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
                }));

                Room room = new Room(roomOpenPacket.roomNumber, roomTitle, roomUserCount);
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
            roomNotify.roomInfo.userCount = 0;

            if (GetRoomOpenNotifyPacket(ref roomNotify, packetHeader.size))
            {
                this.Invoke(new Action(() =>
                {
                    listBox_room.Items.Add(roomNotify.roomInfo.title);
                }));

                Room room = new Room(roomNotify.roomInfo.number, roomNotify.roomInfo.title, roomNotify.roomInfo.userCount);
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
                        button_RoomLeave.Enabled = true;
                        button_chat.Enabled = true;
                        textBox_chat.Enabled = true;
                        listBox_chat.Enabled = true;

                        listBox_user.Enabled = true;
                        listBox_user.Items.Add(userID);
                    }));
                }
                else
                {
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
                        listBox_user.SelectedIndex = listBox_user.Items.Count - 1;
                    }));
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
                        listBox_user.SelectedIndex = listBox_user.Items.Count - 1;
                    }));
                }
            }
        }

        void ProcessRoomLeaveResponse(PACKET_HEADER packetHeader)
        {
        }

        void ProcessRoomLeaveNotify(PACKET_HEADER packetHeader)
        {
        }

        void CheckLimitChatCount()
        {
            if (listBox_chat.Items.Count > 512)
                listBox_chat.Items.Clear();
        }
    }
}
