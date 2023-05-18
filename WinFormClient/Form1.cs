using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace WinFormClient
{
    public partial class Form1 : Form
    {
        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void ChatClientStart([MarshalAs(UnmanagedType.LPWStr)] String ip, [MarshalAs(UnmanagedType.I2)] short port);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendLoginPacket([MarshalAs(UnmanagedType.LPStr)] string id, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder pwd, int idSize, int pwdSize);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendChatPacket([MarshalAs(UnmanagedType.LPStr)] string str, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void GetPacketHeader(ref PACKET_HEADER packetHeader);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern bool GetChatNotifyPacket(ref SC_CHAT_NOTIFY packetData, int size);


        bool IsActivatedBackGroundThread = false;

        Thread BackGroundThread = null;

        public Form1()
        {
            InitializeComponent();
            BackGroundThread = new Thread(BackGroundProcess);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            IsActivatedBackGroundThread = false;
            BackGroundThread.Join();
        }

        private void Button_isConnect_Click(object sender, EventArgs e)
        {
            ChatClientStart(textBox_IP.Text, Int16.Parse(textBox_port.Text));
            IsActivatedBackGroundThread = true;
            BackGroundThread.Start();
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
        
        void BackGroundProcess()
        {
            while (IsActivatedBackGroundThread)
            {
                PACKET_HEADER packetHeader;
                packetHeader.size = 0;
                packetHeader.id = PacketID.LOGIN_REQUEST;
                GetPacketHeader(ref packetHeader);
                if (packetHeader.size != 0)
                {
                    // packet.
                    int size = packetHeader.size;
                    PacketID packetType = packetHeader.id;

                    switch(packetType)
                    {
                        case PacketID.CHAT_RESPONSE:
                            SC_CHAT_NOTIFY chatPacket;
                            chatPacket.message = null;
                            chatPacket.header = packetHeader;
                            if (GetChatNotifyPacket(ref chatPacket, size))
                            {
                                this.Invoke(new Action(() =>
                                {
                                    listBox_chat.Items.Add(chatPacket.message);
                                }));
                            }
                            break;
                    }
                }
            }
        }

        private void button_RoomCreate_Click(object sender, EventArgs e)
        {
            RoomCreator roomCreator = new RoomCreator();
            roomCreator.ShowDialog();
        }
    }
}
