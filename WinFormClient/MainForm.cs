﻿using System;
using System.Text;
using System.Windows.Forms;
using System.Threading;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Drawing;

namespace WinFormClient
{
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

        string userID = null;
        string roomTitle = null;
        int roomUserCount = 0;
        bool[] chatDrawIndexFlags = new bool[512];

        Thread BackGroundRecvThread = null;
        RoomManager roomManager = new RoomManager();

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
            ChatClientStart(textBox_IP.Text, Int16.Parse(textBox_port.Text));
            IsActivatedBackGroundThread = true;
            BackGroundRecvThread.Start();
            SendConnectPacket();
            button_isConnect.Enabled = false;

            // Login Setup
            textBox_ID.Enabled = true;
            textBox_password.Enabled = true;
            button_login.Enabled = true;

            // Lobby Setup
            button_RoomCreate.Enabled = true;
            button_RoomEnter.Enabled = true;
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

        private void ListBox_chat_DrawItem1(object sender, DrawItemEventArgs e)
        {
            e.DrawBackground();

            if (e.Index == -1)
                return;

            if (chatDrawIndexFlags[e.Index])
            {
                StringFormat strFormat = new StringFormat();
                strFormat.Alignment = StringAlignment.Far;
                e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 10, FontStyle.Bold), Brushes.Black, e.Bounds, strFormat);
            }
            else
            {
                e.Graphics.DrawString(listBox_chat.Items[e.Index].ToString(), new Font("Arial", 10, FontStyle.Regular), Brushes.Black, e.Bounds);
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

            if (roomManager.roomDictionary.TryGetValue(listBox_room.Items[index].ToString(), out room))
            {
                SendRoomEnterPacket(room.number);
            }
            else
            {
                MessageBox.Show("Does not exist room.", "Alert", MessageBoxButtons.OK, MessageBoxIcon.Error);
            }
        }

        private void button_RoomLeave_Click(object sender, EventArgs e)
        {

        }

        void BackGroundRecvProcess()
        {
            while (IsActivatedBackGroundThread)
            {
                //this.Invoke(new Action(() =>
                //{
                //    if (listBox_room.SelectedIndex >= 0)
                //    {
                //        button_RoomEnter.Enabled = true;
                //    }
                //    else
                //    {
                //        button_RoomEnter.Enabled = false;
                //    }
                //}));
                ProcessPacket();
            }
        }
    }
}
