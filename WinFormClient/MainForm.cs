using System;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Drawing;

namespace WinFormClient
{
    enum ChatDrawingType
    {
        NONE,
        RESPONSE_CHAT,
        NOTIFY_ID,
        NOTIFY_CHAT,
        INFO,
    }
    public partial class MainForm : Form
    {

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void ChatClientStart([MarshalAs(UnmanagedType.LPWStr)] String ip, [MarshalAs(UnmanagedType.I2)] short port);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendConnectPacket();

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendLoginPacket([MarshalAs(UnmanagedType.LPStr)] string id, int idSize, [MarshalAs(UnmanagedType.LPStr)] string pwd, int pwdSize);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendChatPacket([MarshalAs(UnmanagedType.LPStr)] string str, int size);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomOpenPacket([MarshalAs(UnmanagedType.LPStr)] string str, int titleSize, int userCount);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomEnterPacket(int number);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void SendRoomLeavePacket(int number);

        Thread BackGroundRecvThread = null;
        ChatDrawingType[] chatDrawIndexFlags = new ChatDrawingType[512];

        // 로컬 ChatApplication 정보
        string userID = null;
        string roomTitle = null;
        int roomUserCount = 0;
        RoomManager roomManager = new RoomManager();

        // flag
        bool IsActivatedBackGroundThread = false;
        bool IsActivatedLogin = false;
        bool IsActivatedConnect = false;

        public MainForm()
        {
            InitializeComponent();
            InitProcessPacket();
            BackGroundRecvThread = new Thread(BackGroundRecvProcess);
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            roomManager.roomDictionary.Clear();
            IsActivatedBackGroundThread = false;

            if (BackGroundRecvThread.IsAlive)
                BackGroundRecvThread.Join();
        }

        private void Button_isConnect_Click(object sender, EventArgs e)
        {
            if(!IsActivatedConnect)
            {
                ChatClientStart(textBox_IP.Text, Int16.Parse(textBox_port.Text));
                IsActivatedBackGroundThread = true;
                BackGroundRecvThread.Start();
                SendConnectPacket();

                // Connect Disable
                textBox_IP.Enabled = false;
                textBox_port.Enabled = false;

                // Login Setup
                textBox_ID.Enabled = true;
                textBox_password.Enabled = true;
                button_login.Enabled = true;
                
                IsActivatedConnect = true;
            }
            else
            {
                // Connect Setup
                textBox_IP.Enabled = true;
                textBox_port.Enabled = true;

                // Login Disable
                textBox_ID.Enabled = false;
                textBox_password.Enabled = false;
                button_login.Enabled = false;

                IsActivatedConnect = false;
            }
        }

        private void Button_login_Click(object sender, EventArgs e)
        {
            if (!IsActivatedLogin)
            {
                SendLoginPacket(textBox_ID.Text, textBox_ID.Text.Length, textBox_password.Text, textBox_password.Text.Length);
            }
            else
            {
                userID = null;
                button_login.Text = "Login";
                textBox_ID.Text = "";
                textBox_ID.Enabled = true;
                textBox_password.Enabled = true;
                textBox_password.Text = "";
                IsActivatedLogin = false;
            }
        }

        private void Button_chat_Click(object sender, EventArgs e)
        {
            string chatMsg = textBox_chat.Text;
            SendChatPacket(chatMsg, chatMsg.Length);
            button_chat.Enabled = false;
        }

        private void ListBox_user_DrawEvent(object sender, DrawItemEventArgs e)
        {
            e.DrawBackground();

            if (e.Index == -1)
                return;

            if (e.Index == 0)
                e.Graphics.DrawString(listBox_user.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Bold), Brushes.Black, e.Bounds);
            else
                e.Graphics.DrawString(listBox_user.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Regular), Brushes.Black, e.Bounds);
        }

        private void ListBox_chat_DrawEvent(object sender, DrawItemEventArgs e)
        {
            e.DrawBackground();

            if (e.Index == -1)
                return;
            
            switch (chatDrawIndexFlags[e.Index])
            {
                case ChatDrawingType.RESPONSE_CHAT:
                    StringFormat strFormat = new StringFormat();
                    strFormat.Alignment = StringAlignment.Far;
                    e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Bold), Brushes.Black, e.Bounds, strFormat);
                    break;

                case ChatDrawingType.NOTIFY_ID:
                    e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Bold), Brushes.Black, e.Bounds);
                    break;

                case ChatDrawingType.NOTIFY_CHAT:
                    e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Regular), Brushes.Black, e.Bounds);
                    break;

                case ChatDrawingType.INFO:
                    StringFormat strInfoFormat = new StringFormat();
                    strInfoFormat.Alignment = StringAlignment.Center;
                    e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 9, FontStyle.Regular), Brushes.Black, e.Bounds, strInfoFormat);
                    //D3D3D3
                    break;
            }
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
            int index = listBox_room.SelectedIndex;
            if (index == -1)
                return;

            if (roomManager.roomDictionary.TryGetValue(listBox_room.Items[index].ToString(), out room))
            {
                SendRoomEnterPacket(room.number);
            }
            else
            {
                MessageBox.Show("Does not exist room.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }

            button_RoomEnter.Enabled = false;
        }

        private void button_RoomLeave_Click(object sender, EventArgs e)
        {
            Room room;
            int index = listBox_room.SelectedIndex;
            if (index == -1)
                return;

            if (roomManager.roomDictionary.TryGetValue(listBox_room.Items[index].ToString(), out room))
            {
                SendRoomLeavePacket(room.number);
            }
            else
            {
                MessageBox.Show("Does not exist room.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        void BackGroundRecvProcess()
        {
            while (IsActivatedBackGroundThread)
            { 
                ProcessPacket();
            }
        }
    }
}
