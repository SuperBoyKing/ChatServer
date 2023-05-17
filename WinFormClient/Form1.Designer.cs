
namespace WinFormClient
{
    partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.label1 = new System.Windows.Forms.Label();
            this.textBox_IP = new System.Windows.Forms.TextBox();
            this.textBox_port = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.button_isConnect = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.button_login = new System.Windows.Forms.Button();
            this.textBox_password = new System.Windows.Forms.TextBox();
            this.pw = new System.Windows.Forms.Label();
            this.textBox_ID = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.button_chat = new System.Windows.Forms.Button();
            this.textBox_chat = new System.Windows.Forms.TextBox();
            this.listBox_chat = new System.Windows.Forms.ListBox();
            this.listBox_user = new System.Windows.Forms.ListBox();
            this.label_ConnectStatus = new System.Windows.Forms.Label();
            this.listBox_roomList = new System.Windows.Forms.ListBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.Lobby = new System.Windows.Forms.GroupBox();
            this.button_RoomEnter = new System.Windows.Forms.Button();
            this.label4 = new System.Windows.Forms.Label();
            this.textbox_roomName = new System.Windows.Forms.TextBox();
            this.textBox1 = new System.Windows.Forms.TextBox();
            this.label5 = new System.Windows.Forms.Label();
            this.button_RoomLeave = new System.Windows.Forms.Button();
            this.button_RoomCreate = new System.Windows.Forms.Button();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.Lobby.SuspendLayout();
            this.SuspendLayout();
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(15, 15);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(65, 15);
            this.label1.TabIndex = 0;
            this.label1.Text = "Server IP : ";
            // 
            // textBox_IP
            // 
            this.textBox_IP.Location = new System.Drawing.Point(81, 13);
            this.textBox_IP.Name = "textBox_IP";
            this.textBox_IP.Size = new System.Drawing.Size(144, 23);
            this.textBox_IP.TabIndex = 1;
            // 
            // textBox_port
            // 
            this.textBox_port.Location = new System.Drawing.Point(325, 14);
            this.textBox_port.Name = "textBox_port";
            this.textBox_port.Size = new System.Drawing.Size(81, 23);
            this.textBox_port.TabIndex = 3;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(242, 17);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(77, 15);
            this.label2.TabIndex = 2;
            this.label2.Text = "Server Port : ";
            // 
            // button_isConnect
            // 
            this.button_isConnect.Location = new System.Drawing.Point(426, 14);
            this.button_isConnect.Name = "button_isConnect";
            this.button_isConnect.Size = new System.Drawing.Size(155, 23);
            this.button_isConnect.TabIndex = 4;
            this.button_isConnect.Text = "Connect";
            this.button_isConnect.UseVisualStyleBackColor = true;
            this.button_isConnect.Click += new System.EventHandler(this.Button_isConnect_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.button_login);
            this.groupBox1.Controls.Add(this.textBox_password);
            this.groupBox1.Controls.Add(this.pw);
            this.groupBox1.Controls.Add(this.textBox_ID);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Location = new System.Drawing.Point(15, 55);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(183, 160);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Login";
            // 
            // button_login
            // 
            this.button_login.Location = new System.Drawing.Point(9, 121);
            this.button_login.Name = "button_login";
            this.button_login.Size = new System.Drawing.Size(168, 25);
            this.button_login.TabIndex = 6;
            this.button_login.Text = "Login";
            this.button_login.UseVisualStyleBackColor = true;
            this.button_login.Click += new System.EventHandler(this.Button_login_Click);
            // 
            // textBox_password
            // 
            this.textBox_password.Location = new System.Drawing.Point(42, 79);
            this.textBox_password.Name = "textBox_password";
            this.textBox_password.Size = new System.Drawing.Size(132, 23);
            this.textBox_password.TabIndex = 9;
            // 
            // pw
            // 
            this.pw.AutoSize = true;
            this.pw.Location = new System.Drawing.Point(6, 82);
            this.pw.Name = "pw";
            this.pw.Size = new System.Drawing.Size(32, 15);
            this.pw.TabIndex = 8;
            this.pw.Text = "PW :";
            // 
            // textBox_ID
            // 
            this.textBox_ID.Location = new System.Drawing.Point(42, 37);
            this.textBox_ID.Name = "textBox_ID";
            this.textBox_ID.Size = new System.Drawing.Size(132, 23);
            this.textBox_ID.TabIndex = 7;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 40);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(30, 15);
            this.label3.TabIndex = 6;
            this.label3.Text = "ID  :";
            // 
            // textBox3
            // 
            this.textBox3.Location = new System.Drawing.Point(42, 31);
            this.textBox3.Name = "textBox3";
            this.textBox3.Size = new System.Drawing.Size(100, 23);
            this.textBox3.TabIndex = 1;
            // 
            // button_chat
            // 
            this.button_chat.Location = new System.Drawing.Point(479, 415);
            this.button_chat.Name = "button_chat";
            this.button_chat.Size = new System.Drawing.Size(81, 24);
            this.button_chat.TabIndex = 8;
            this.button_chat.Text = "chat";
            this.button_chat.UseVisualStyleBackColor = true;
            this.button_chat.Click += new System.EventHandler(this.Button_chat_Click);
            // 
            // textBox_chat
            // 
            this.textBox_chat.Location = new System.Drawing.Point(197, 416);
            this.textBox_chat.MaxLength = 256;
            this.textBox_chat.Multiline = true;
            this.textBox_chat.Name = "textBox_chat";
            this.textBox_chat.Size = new System.Drawing.Size(276, 23);
            this.textBox_chat.TabIndex = 9;
            // 
            // listBox_chat
            // 
            this.listBox_chat.FormattingEnabled = true;
            this.listBox_chat.ItemHeight = 15;
            this.listBox_chat.Location = new System.Drawing.Point(197, 60);
            this.listBox_chat.Name = "listBox_chat";
            this.listBox_chat.Size = new System.Drawing.Size(363, 349);
            this.listBox_chat.TabIndex = 10;
            this.listBox_chat.TabStop = false;
            // 
            // listBox_user
            // 
            this.listBox_user.FormattingEnabled = true;
            this.listBox_user.ItemHeight = 15;
            this.listBox_user.Location = new System.Drawing.Point(6, 60);
            this.listBox_user.Name = "listBox_user";
            this.listBox_user.Size = new System.Drawing.Size(185, 379);
            this.listBox_user.TabIndex = 11;
            this.listBox_user.TabStop = false;
            // 
            // label_ConnectStatus
            // 
            this.label_ConnectStatus.AutoSize = true;
            this.label_ConnectStatus.Location = new System.Drawing.Point(15, 679);
            this.label_ConnectStatus.Name = "label_ConnectStatus";
            this.label_ConnectStatus.Size = new System.Drawing.Size(92, 15);
            this.label_ConnectStatus.TabIndex = 12;
            this.label_ConnectStatus.Text = "Connect Status:";
            // 
            // listBox_roomList
            // 
            this.listBox_roomList.FormattingEnabled = true;
            this.listBox_roomList.ItemHeight = 15;
            this.listBox_roomList.Location = new System.Drawing.Point(16, 22);
            this.listBox_roomList.Name = "listBox_roomList";
            this.listBox_roomList.Size = new System.Drawing.Size(179, 124);
            this.listBox_roomList.TabIndex = 13;
            this.listBox_roomList.TabStop = false;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.textBox1);
            this.groupBox2.Controls.Add(this.textbox_roomName);
            this.groupBox2.Controls.Add(this.label4);
            this.groupBox2.Controls.Add(this.listBox_user);
            this.groupBox2.Controls.Add(this.textBox_chat);
            this.groupBox2.Controls.Add(this.listBox_chat);
            this.groupBox2.Controls.Add(this.button_chat);
            this.groupBox2.Location = new System.Drawing.Point(15, 221);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(566, 454);
            this.groupBox2.TabIndex = 14;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Room";
            // 
            // Lobby
            // 
            this.Lobby.Controls.Add(this.button_RoomCreate);
            this.Lobby.Controls.Add(this.button_RoomLeave);
            this.Lobby.Controls.Add(this.button_RoomEnter);
            this.Lobby.Controls.Add(this.listBox_roomList);
            this.Lobby.Location = new System.Drawing.Point(212, 55);
            this.Lobby.Name = "Lobby";
            this.Lobby.Size = new System.Drawing.Size(369, 160);
            this.Lobby.TabIndex = 15;
            this.Lobby.TabStop = false;
            this.Lobby.Text = "Lobby";
            // 
            // button_RoomEnter
            // 
            this.button_RoomEnter.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.button_RoomEnter.Location = new System.Drawing.Point(209, 22);
            this.button_RoomEnter.Name = "button_RoomEnter";
            this.button_RoomEnter.Size = new System.Drawing.Size(145, 33);
            this.button_RoomEnter.TabIndex = 10;
            this.button_RoomEnter.Text = "Enter";
            this.button_RoomEnter.UseVisualStyleBackColor = true;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(98, 25);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(86, 15);
            this.label4.TabIndex = 12;
            this.label4.Text = "Room Name : ";
            // 
            // textbox_roomName
            // 
            this.textbox_roomName.Location = new System.Drawing.Point(190, 20);
            this.textbox_roomName.Name = "textbox_roomName";
            this.textbox_roomName.ReadOnly = true;
            this.textbox_roomName.Size = new System.Drawing.Size(100, 23);
            this.textbox_roomName.TabIndex = 13;
            this.textbox_roomName.TabStop = false;
            // 
            // textBox1
            // 
            this.textBox1.Location = new System.Drawing.Point(406, 22);
            this.textBox1.Name = "textBox1";
            this.textBox1.ReadOnly = true;
            this.textBox1.Size = new System.Drawing.Size(49, 23);
            this.textBox1.TabIndex = 14;
            this.textBox1.TabStop = false;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(356, 25);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(46, 15);
            this.label5.TabIndex = 15;
            this.label5.Text = "Users : ";
            // 
            // button_RoomLeave
            // 
            this.button_RoomLeave.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.button_RoomLeave.Location = new System.Drawing.Point(209, 69);
            this.button_RoomLeave.Name = "button_RoomLeave";
            this.button_RoomLeave.Size = new System.Drawing.Size(145, 33);
            this.button_RoomLeave.TabIndex = 14;
            this.button_RoomLeave.Text = "Leave";
            this.button_RoomLeave.UseVisualStyleBackColor = true;
            // 
            // button_RoomCreate
            // 
            this.button_RoomCreate.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.button_RoomCreate.Location = new System.Drawing.Point(209, 113);
            this.button_RoomCreate.Name = "button_RoomCreate";
            this.button_RoomCreate.Size = new System.Drawing.Size(145, 33);
            this.button_RoomCreate.TabIndex = 15;
            this.button_RoomCreate.Text = "Create";
            this.button_RoomCreate.UseVisualStyleBackColor = true;
            this.button_RoomCreate.UseWaitCursor = true;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(594, 699);
            this.Controls.Add(this.Lobby);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.label_ConnectStatus);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.button_isConnect);
            this.Controls.Add(this.textBox_port);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_IP);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1_FormClosing);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.Lobby.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox textBox_IP;
        private System.Windows.Forms.TextBox textBox_port;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button button_isConnect;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox textBox_password;
        private System.Windows.Forms.Label pw;
        private System.Windows.Forms.TextBox textBox_ID;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.Button button_login;
        private System.Windows.Forms.Button button_chat;
        private System.Windows.Forms.TextBox textBox_chat;
        private System.Windows.Forms.ListBox listBox_chat;
        private System.Windows.Forms.ListBox listBox_user;
        private System.Windows.Forms.Label label_ConnectStatus;
        private System.Windows.Forms.ListBox listBox_roomList;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox Lobby;
        private System.Windows.Forms.Button button_RoomEnter;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textBox1;
        private System.Windows.Forms.TextBox textbox_roomName;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button button_RoomCreate;
        private System.Windows.Forms.Button button_RoomLeave;
    }
}

