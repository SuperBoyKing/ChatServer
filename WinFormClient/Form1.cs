using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Collections.Generic;

namespace WinFormClient
{
    public partial class Form1 : Form
    {
        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void ChatClientStart([MarshalAs(UnmanagedType.LPWStr)] String ip, [MarshalAs(UnmanagedType.I2)] short port);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendConnectPacket();

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendLoginPacket([MarshalAs(UnmanagedType.LPStr)] string id, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder pwd, int idSize, int pwdSize);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendChatPacket([MarshalAs(UnmanagedType.LPStr)] string str, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomOpenPacket([MarshalAs(UnmanagedType.LPStr)] string str, int titleSize, int userCount);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomEnterPacket(int number);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetConnectPacket([In, Out, MarshalAs(UnmanagedType.LPArray, ArraySubType = UnmanagedType.LPStruct, SizeParamIndex = 1)]
        ref Room[] packetData, ref int arrayLength, int size);

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

        public string roomTitle = null;
        public int roomUserCount = 0;
        bool IsActivatedBackGroundThread = false;
        Thread BackGroundRecvThread = null;
        RoomManager roomManager = new RoomManager();

        public Form1()
        {
            InitializeComponent();
            BackGroundRecvThread = new Thread(BackGroundRecvProcess);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            roomManager.roomDictionary.Clear();
            IsActivatedBackGroundThread = false;
            BackGroundRecvThread.Join();
        }

        private void Button_isConnect_Click(object sender, EventArgs e)
        {
            ChatClientStart(textBox_IP.Text, Int16.Parse(textBox_port.Text));
            IsActivatedBackGroundThread = true;
            BackGroundRecvThread.Start();
            SendConnectPacket();
        }

        private void Button_login_Click(object sender, EventArgs e)
        {
            //SendLoginPacket()
        }

        private void Button_chat_Click(object sender, EventArgs e)
        {
            string chatMsg = textBox_chat.Text;
            SendChatPacket(chatMsg, chatMsg.Length);
            listBox_chat.Items.Add(chatMsg);
        }

        private void button_RoomCreate_Click(object sender, EventArgs e)
        {
            RoomCreator roomCreator = new RoomCreator();
            var result = roomCreator.ShowDialog();

            if (result == DialogResult.OK)
            {
                roomTitle = roomCreator.returnTitle;
                roomUserCount = roomCreator.returnUserCount;
                if (!roomManager.roomDictionary.ContainsKey(roomTitle))
                {
                    SendRoomOpenPacket(roomTitle, roomTitle.Length, roomUserCount);
                    button_RoomCreate.Enabled = false;
                }
            }
        }

        private void button_RoomEnter_Click(object sender, EventArgs e)
        {
            Room room;
            roomManager.roomDictionary.TryGetValue(listBox_room.SelectedIndices.ToString(), out room);
            //SendRoomEnterPacket()
        }

        private void button_RoomLeave_Click(object sender, EventArgs e)
        {

        }

        void BackGroundRecvProcess()
        {
            while (IsActivatedBackGroundThread)
            {
                PACKET_HEADER packetHeader;
                packetHeader.size = 0;
                packetHeader.id = PacketID.LOGIN_REQUEST;
                if (GetPacketHeader(ref packetHeader))
                {
                    // packet.
                    int size = packetHeader.size;
                    PacketID packetType = packetHeader.id;

                    switch (packetType)
                    {
                        case PacketID.CONNECT_RESPONSE:
                            SC_CONNECT_RESPONSE connectResponsePacket;
                            connectResponsePacket.header = packetHeader;
                            connectResponsePacket.roomInfo = new Room[10];
                            int length = 0;
                            if (GetConnectPacket(ref connectResponsePacket.roomInfo, ref length, size))
                            {
                                for (int i = 0; i < length; ++i) 
                                {
                                    roomManager.roomDictionary.Add(connectResponsePacket.roomInfo[i].title, connectResponsePacket.roomInfo[i]);
                                    this.Invoke(new Action(() =>
                                    {
                                        listBox_room.Items.Add(connectResponsePacket.roomInfo[i].title);
                                    }));
                                }
                            }
                            break;

                        case PacketID.CHAT_RESPONSE:
                            break;

                        case PacketID.CHAT_NOTIFY:
                            SC_CHAT_NOTIFY chatPacket;
                            chatPacket.header = packetHeader;
                            chatPacket.message = null;

                            if (GetChatNotifyPacket(ref chatPacket, size))
                            {
                                this.Invoke(new Action(() =>
                                {
                                    listBox_chat.Items.Add(chatPacket.message);
                                }));
                            }
                            break;

                        case PacketID.ROOM_OPEN_RESPONSE:
                            SC_ROOM_OPEN_RESPONSE roomOpenPacket;
                            roomOpenPacket.header = packetHeader;
                            roomOpenPacket.roomNumber = 0;
                            roomOpenPacket.result = false;

                            if (GetRoomOpenPacket(ref roomOpenPacket, size))
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

                            break;

                        case PacketID.ROOM_OPEN_NOTIFY:
                            SC_ROOM_OPEN_NOTIFY roomNotify;
                            roomNotify.roomInfo.number = 0;
                            roomNotify.roomInfo.title = null;
                            roomNotify.roomInfo.userCount = 0;

                            if (GetRoomOpenNotifyPacket(ref roomNotify, size))
                            {
                                this.Invoke(new Action(() =>
                                {
                                    listBox_room.Items.Add(roomNotify.roomInfo.title);
                                }));

                                Room room = new Room(roomNotify.roomInfo.number, roomNotify.roomInfo.title, roomNotify.roomInfo.userCount);
                                roomManager.roomDictionary.Add(roomNotify.roomInfo.title, room);
                            }
                            break;

                        case PacketID.ROOM_ENTER_RESPONSE:
                            SC_ROOM_ENTER_RESPONSE roomEnterPacket;
                            roomEnterPacket.header = packetHeader;
                            roomEnterPacket.result = false;

                            if (GetRoomEnterPacket(ref roomEnterPacket, size))
                            {
                                this.Invoke(new Action(() =>
                                {
                                    
                                }));
                            }
                            break;
                    }
                }
                else { }
            }
        }
    }
}
