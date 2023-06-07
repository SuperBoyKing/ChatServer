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
        static extern bool GetPacket(byte[] packetData, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetRoomUserNotifyPacket([In, Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStr, SizeParamIndex = 1)]
            ref SC_USER_LIST_NOTIFY_MULTIPLE[] packetData, int size);

        public static byte[] StructToByte(object obj, int count = 1)
        {
            int size = Marshal.SizeOf(obj);
            byte[] arr = new byte[size];
            IntPtr ptr = Marshal.AllocHGlobal(size);

            Marshal.StructureToPtr(obj, ptr, true);
            Marshal.Copy(ptr, arr, 0, size);
            Marshal.FreeHGlobal(ptr);
            return arr;
        }

        public static T ByteToStruct<T>(byte[] buffer) where T : struct
        {
            int size = Marshal.SizeOf(typeof(T));

            if (size > buffer.Length)
            {
                throw new Exception();
            }

            IntPtr ptr = Marshal.AllocHGlobal(size);
            Marshal.Copy(buffer, 0, ptr, size);
            T obj = (T)Marshal.PtrToStructure(ptr, typeof(T));
            Marshal.FreeHGlobal(ptr);
            return obj;
        }

        Dictionary<PacketID, Action<PACKET_HEADER>> PacketFuncDictionary = new Dictionary<PacketID, Action<PACKET_HEADER>>();

        void InitProcessPacket()
        {
            PacketFuncDictionary.Add(PacketID.CONNECT_RESPONSE, ProcessConnectResponse);
            PacketFuncDictionary.Add(PacketID.REGISTER_RESPONSE, ProcessRegisterResponse);
            PacketFuncDictionary.Add(PacketID.LOGIN_RESPONSE, ProcessLoginResponse);
            PacketFuncDictionary.Add(PacketID.LOGOUT_RESPONSE, ProcessLogoutResponse);
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

        void ProcessRegisterResponse(PACKET_HEADER packetHeader)
        {
            SC_REGISTER_RESPONSE registerPacket;
            registerPacket.result = false;

            byte[] data = StructToByte(registerPacket);

            if (GetPacket(data, packetHeader.size))
            {
                registerPacket = ByteToStruct<SC_REGISTER_RESPONSE>(data);
                if (registerPacket.result)
                {
                    MessageBox.Show("Register Success.", "Regit", MessageBoxButtons.OK, MessageBoxIcon.Information);
                }
                else
                {
                    MessageBox.Show("Register Failed.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }

        }

        void ProcessLoginResponse(PACKET_HEADER packetHeader)
        {
            SC_LOGIN_RESPONSE loginResPacket;
            loginResPacket.result = false;

            byte[] data = StructToByte(loginResPacket);

            if (GetPacket(data, packetHeader.size))
            {
                loginResPacket = ByteToStruct<SC_LOGIN_RESPONSE>(data);
                if (loginResPacket.result)
                {
                    SetLoginStateUI();
                    SendConnectPacket();
                }
                else
                {
                    MessageBox.Show("Login Failed.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
                }
            }
        }

        void ProcessLogoutResponse(PACKET_HEADER packetHeader)
        {
            SC_LOGOUT_RESPONSE logoutPacket;
            logoutPacket.result = false;

            byte[] data = StructToByte(logoutPacket);

            if (GetPacket(data, packetHeader.size))
            {
                logoutPacket = ByteToStruct<SC_LOGOUT_RESPONSE>(data);
                if (logoutPacket.result == true)
                {
                    DisableLoginStateUI();
                }
            }

        }

        void ProcessChatResponse(PACKET_HEADER packetHeader) 
        {
            SC_CHAT_RESPONSE chatResPacket;
            chatResPacket.result = false;

            byte[] data = StructToByte(chatResPacket);

            if (GetPacket(data, packetHeader.size))
            {
                chatResPacket = ByteToStruct<SC_CHAT_RESPONSE>(data);
                if (chatResPacket.result)
                    AddChatMsgUI();
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

            byte[] data = StructToByte(chatNotifyPacket);

            if (GetPacket(data, packetHeader.size))
            {
                chatNotifyPacket = ByteToStruct<SC_CHAT_NOTIFY>(data);
                AddChatMsgUI(chatNotifyPacket);
            }
        }

        void ProcessRoomOpenResponse(PACKET_HEADER packetHeader)
        {
            SC_ROOM_OPEN_RESPONSE roomOpenPacket;
            roomOpenPacket.roomNumber = 0;
            roomOpenPacket.result = false;

            byte[] data = StructToByte(roomOpenPacket);

            if (GetPacket(data, packetHeader.size))
            {
                roomOpenPacket = ByteToStruct<SC_ROOM_OPEN_RESPONSE>(data);
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

            byte[] data = StructToByte(roomNotify);

            if (GetPacket(data, packetHeader.size))
            {
                roomNotify = ByteToStruct<SC_ROOM_OPEN_NOTIFY_MULTIPLE>(data);
                AddRoomListUI(roomNotify.roomInfo);
            }
        }

        void ProcessRoomEnterResponse(PACKET_HEADER packetHeader)
        {
            SC_ROOM_ENTER_RESPONSE roomEnterPacket;
            roomEnterPacket.result = false;
            roomEnterPacket.currentUserCount = 0;

            byte[] data = StructToByte(roomEnterPacket);

            if (GetPacket(data, packetHeader.size))
            {
                roomEnterPacket = ByteToStruct<SC_ROOM_ENTER_RESPONSE>(data);
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

            byte[] data = StructToByte(roomEnterUserPacket);

            if (GetPacket(data, packetHeader.size))
            {
                roomEnterUserPacket = ByteToStruct<SC_ROOM_ENTER_USER_NOTIFY>(data);
                for (int i = 0; i < packetHeader.packetCount; ++i)
                {
                    AddUserListUI(roomEnterUserPacket.userID);
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

            byte[] data = StructToByte(roomLeavePacket);

            if (GetPacket(data, packetHeader.size))
            {
                roomLeavePacket = ByteToStruct<SC_ROOM_LEAVE_RESPONSE>(data);
                if (roomLeavePacket.result)
                {
                    DisableRoomUI();

                    for (int i = 0; i < listBox_chat.Items.Count; ++i)
                        chatDrawIndexFlags[i] = ChatDrawingType.NONE;
                }
            }
        }

        void ProcessRoomLeaveNotify(PACKET_HEADER packetHeader)
        {
            SC_ROOM_LEAVE_USER_NOTIFY roomLeaveNotify;
            roomLeaveNotify.userID = null;

            byte[] data = StructToByte(roomLeaveNotify);

            if (GetPacket(data, packetHeader.size))
            {
                roomLeaveNotify = ByteToStruct<SC_ROOM_LEAVE_USER_NOTIFY>(data);
                RemoveUserListUI(roomLeaveNotify.userID);
            }
        }

        void ProcessRoomClose(PACKET_HEADER packetHeader)
        {
            SC_ROOM_CLOSE closePacket;
            closePacket.roomNumber = 0;

            byte[] data = StructToByte(closePacket);

            if (GetPacket(data, packetHeader.size))
            {
                closePacket = ByteToStruct<SC_ROOM_CLOSE>(data);
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
