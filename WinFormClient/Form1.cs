using System;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;
using System.Threading;

namespace WinFormClient
{
    public partial class Form1 : Form
    {
        [DllImport("msvcrt.dll", SetLastError = false)]
        static extern IntPtr memcpy(IntPtr dest, IntPtr src, int count);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void ChatInit([MarshalAs(UnmanagedType.LPWStr)] String ip, [MarshalAs(UnmanagedType.I2)] short port);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void ChatLogin([MarshalAs(UnmanagedType.LPStr)] string id, [MarshalAs(UnmanagedType.LPWStr)] StringBuilder pwd, int idSize, int pwdSize);

        [DllImport("ChatClient.dll", CallingConvention = CallingConvention.Cdecl)]
        static extern void ChatMsg([MarshalAs(UnmanagedType.LPStr)] string str, int size);

        bool IsActivatedBackGroundThread = false;

        Thread BackGroundThread = null;

        public Form1()
        {
            InitializeComponent();
            BackGroundThread = new Thread(BackGroundProcess);
        }

        private void Button_isConnect_Click(object sender, EventArgs e)
        {
            ChatInit(textBox_IP.Text, Int16.Parse(textBox_port.Text));
            IsActivatedBackGroundThread = true;
            BackGroundThread.Start();
        }

        private void Button_login_Click(object sender, EventArgs e)
        {
            //ChatLogin()
        }

        private void Button_chat_Click(object sender, EventArgs e)
        {
            string chatMsg = textBox_chat.Text;
            ChatMsg(chatMsg, chatMsg.Length);
            listBox_chat.Items.Add(chatMsg);
            //listBox_chat.SelectedIndex = listBox_chat.Items.Count - 1;
        }
        
        void BackGroundProcess()
        {
            while (IsActivatedBackGroundThread)
            {
                unsafe
                {
                   
                }
            }
        }
    }
}
