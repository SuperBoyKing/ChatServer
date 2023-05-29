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
                    AddRoomListUI(conResPacket[i].roomInfo);
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
                    SetLoginStateUI();
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
                if (roomOpenPacket.result)
                {
                    Room room = new Room(roomOpenPacket.roomNumber, roomTitle, 0, roomMaxUserCount);
                    AddRoomListUI(room);
                }
            }
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
                AddRoomListUI(roomNotify.roomInfo);
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
                    EnableRoomUI();
                    AddUserListUI(userID);
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
                    AddUserListUI(roomEnterUserPacket.userID);
                    UserCountIncreaseUI();
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
                    AddUserListUI(roomUserListPacket[i].userID);
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

                    DisableRoomUI();
                }
            }
        }

        void ProcessRoomLeaveNotify(PACKET_HEADER packetHeader)
        {
            SC_ROOM_LEAVE_USER_NOTIFY roomLeaveNotify;
            roomLeaveNotify.userID = null;

            if (GetRoomLeaveNotifyPacket(ref roomLeaveNotify, packetHeader.size))
            {
                RemoveUserListUI(roomLeaveNotify.userID);
            }
        }

        void ProcessRoomClose(PACKET_HEADER packetHeader)
        {
            SC_ROOM_CLOSE closePacket;
            closePacket.roomNumber = 0;

            if (GetClosePacket(ref closePacket, packetHeader.size))
            {
                RemoveRoomListUI(closePacket.roomNumber);
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
