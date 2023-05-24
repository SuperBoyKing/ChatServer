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
        ref SC_CONNECT_RESPONSE[] packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetPacketHeader(ref PACKET_HEADER packetHeader);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetChatNotifyPacket(ref SC_CHAT_NOTIFY packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomOpenPacket(ref SC_ROOM_OPEN_RESPONSE packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomOpenNotifyPacket(ref SC_ROOM_OPEN_NOTIFY packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomEnterPacket(ref SC_ROOM_ENTER_RESPONSE packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomUserNotifyPacket([In, Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr, SizeParamIndex = 1)]
            ref SC_ROOM_USERLIST_NOTIFY[] packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomEnterUserNotify(ref SC_ROOM_ENTER_USER_NOTIFY packetData, int size);

        Dictionary<PacketID, Action<PACKET_HEADER>> PacketFuncDictionary = new Dictionary<PacketID, Action<PACKET_HEADER>>();

        void InitProcessPacket()
        {
            PacketFuncDictionary.Add(PacketID.CONNECT_RESPONSE, ProcessConnectResponse);
            PacketFuncDictionary.Add(PacketID.CHAT_RESPONSE, ProcessChatResponse);
            PacketFuncDictionary.Add(PacketID.CHAT_NOTIFY, ProcessChatNotify);
            PacketFuncDictionary.Add(PacketID.ROOM_OPEN_RESPONSE, ProcessRoomOpenResponse);
            PacketFuncDictionary.Add(PacketID.ROOM_OPEN_NOTIFY, ProcessRoomOpenNotify);
            PacketFuncDictionary.Add(PacketID.ROOM_ENTER_RESPONSE, ProcessRoomEnterResponse);
            PacketFuncDictionary.Add(PacketID.ROOM_LEAVE_RESPONSE, ProcessRoomLeaveResponse);
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
            SC_CONNECT_RESPONSE[] conResPacket = new SC_CONNECT_RESPONSE[packetHeader.packetCount];

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

        void ProcessChatResponse(PACKET_HEADER packetHeader) { }

        void ProcessChatNotify(PACKET_HEADER packetHeader) { }

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
                button_RoomEnter.Enabled = true;
            }));
        }

        void ProcessRoomOpenNotify(PACKET_HEADER packetHeader)
        {
            SC_ROOM_OPEN_NOTIFY roomNotify;
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
                        button_chat.Enabled = true;
                        textBox_chat.Enabled = true;
                        listBox_chat.Enabled = true;

                        listBox_user.Enabled = true;
                        listBox_user.Items.Add("firstUser");
                        listBox_user.SelectedIndex = listBox_user.Items.Count - 1;
                    }));
                }
                else
                {
                    MessageBox.Show("Chat Room is full.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
            
        }

        void ProcessRoomUserListNotify(PACKET_HEADER packetHeader)
        {
            SC_ROOM_USERLIST_NOTIFY[] roomUserListPacket = new SC_ROOM_USERLIST_NOTIFY[packetHeader.packetCount];

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


        void ProcessRoomLeaveResponse(PACKET_HEADER packetHeader)
        {

        }
    }
}
