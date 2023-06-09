﻿
using System.Windows.Forms;

namespace WinFormClient
{
    partial class MainForm
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
            this.button_register = new System.Windows.Forms.Button();
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
            this.label_Status = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.label5 = new System.Windows.Forms.Label();
            this.textBox_roomNumber = new System.Windows.Forms.TextBox();
            this.textBox_roomTitle = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.Lobby = new System.Windows.Forms.GroupBox();
            this.listView_room = new System.Windows.Forms.ListView();
            this.button_RoomCreate = new System.Windows.Forms.Button();
            this.button_RoomLeave = new System.Windows.Forms.Button();
            this.button_RoomEnter = new System.Windows.Forms.Button();
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
            this.textBox_IP.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBox_IP.Location = new System.Drawing.Point(81, 13);
            this.textBox_IP.Name = "textBox_IP";
            this.textBox_IP.Size = new System.Drawing.Size(144, 23);
            this.textBox_IP.TabIndex = 1;
            this.textBox_IP.Text = "127.0.0.1";
            this.textBox_IP.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_IP_KeyPress);
            // 
            // textBox_port
            // 
            this.textBox_port.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBox_port.Location = new System.Drawing.Point(325, 14);
            this.textBox_port.MaxLength = 5;
            this.textBox_port.Name = "textBox_port";
            this.textBox_port.Size = new System.Drawing.Size(81, 23);
            this.textBox_port.TabIndex = 3;
            this.textBox_port.Text = "2023";
            this.textBox_port.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_port_KeyPress);
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
            this.groupBox1.Controls.Add(this.button_register);
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
            // button_register
            // 
            this.button_register.Enabled = false;
            this.button_register.Location = new System.Drawing.Point(100, 122);
            this.button_register.Name = "button_register";
            this.button_register.Size = new System.Drawing.Size(74, 25);
            this.button_register.TabIndex = 10;
            this.button_register.Text = "Register";
            this.button_register.UseVisualStyleBackColor = true;
            this.button_register.Click += new System.EventHandler(this.button_register_Click);
            // 
            // button_login
            // 
            this.button_login.Enabled = false;
            this.button_login.Location = new System.Drawing.Point(9, 122);
            this.button_login.Name = "button_login";
            this.button_login.Size = new System.Drawing.Size(74, 25);
            this.button_login.TabIndex = 6;
            this.button_login.Text = "Login";
            this.button_login.UseVisualStyleBackColor = true;
            this.button_login.Click += new System.EventHandler(this.Button_login_Click);
            // 
            // textBox_password
            // 
            this.textBox_password.Enabled = false;
            this.textBox_password.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBox_password.Location = new System.Drawing.Point(42, 79);
            this.textBox_password.Name = "textBox_password";
            this.textBox_password.PasswordChar = '*';
            this.textBox_password.Size = new System.Drawing.Size(132, 23);
            this.textBox_password.TabIndex = 9;
            this.textBox_password.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_password_KeyPress);
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
            this.textBox_ID.Enabled = false;
            this.textBox_ID.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBox_ID.Location = new System.Drawing.Point(42, 37);
            this.textBox_ID.Name = "textBox_ID";
            this.textBox_ID.Size = new System.Drawing.Size(132, 23);
            this.textBox_ID.TabIndex = 7;
            this.textBox_ID.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_ID_KeyPress);
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
            this.button_chat.Enabled = false;
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
            this.textBox_chat.Enabled = false;
            this.textBox_chat.ImeMode = System.Windows.Forms.ImeMode.Disable;
            this.textBox_chat.Location = new System.Drawing.Point(197, 416);
            this.textBox_chat.MaxLength = 256;
            this.textBox_chat.Multiline = true;
            this.textBox_chat.Name = "textBox_chat";
            this.textBox_chat.Size = new System.Drawing.Size(276, 23);
            this.textBox_chat.TabIndex = 9;
            this.textBox_chat.KeyDown += new System.Windows.Forms.KeyEventHandler(this.textBox_chat_KeyDown);
            // 
            // listBox_chat
            // 
            this.listBox_chat.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.listBox_chat.Enabled = false;
            this.listBox_chat.FormattingEnabled = true;
            this.listBox_chat.ItemHeight = 15;
            this.listBox_chat.Location = new System.Drawing.Point(197, 60);
            this.listBox_chat.Name = "listBox_chat";
            this.listBox_chat.SelectionMode = System.Windows.Forms.SelectionMode.None;
            this.listBox_chat.Size = new System.Drawing.Size(363, 349);
            this.listBox_chat.TabIndex = 10;
            this.listBox_chat.TabStop = false;
            this.listBox_chat.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.ListBox_chat_DrawEvent);
            // 
            // listBox_user
            // 
            this.listBox_user.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.listBox_user.Enabled = false;
            this.listBox_user.FormattingEnabled = true;
            this.listBox_user.ItemHeight = 15;
            this.listBox_user.Location = new System.Drawing.Point(6, 60);
            this.listBox_user.Name = "listBox_user";
            this.listBox_user.SelectionMode = System.Windows.Forms.SelectionMode.None;
            this.listBox_user.Size = new System.Drawing.Size(185, 379);
            this.listBox_user.TabIndex = 11;
            this.listBox_user.TabStop = false;
            this.listBox_user.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.ListBox_user_DrawEvent);
            // 
            // label_Status
            // 
            this.label_Status.AutoSize = true;
            this.label_Status.Location = new System.Drawing.Point(15, 679);
            this.label_Status.Name = "label_Status";
            this.label_Status.Size = new System.Drawing.Size(51, 15);
            this.label_Status.TabIndex = 12;
            this.label_Status.Text = "Status : ";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.label5);
            this.groupBox2.Controls.Add(this.textBox_roomNumber);
            this.groupBox2.Controls.Add(this.textBox_roomTitle);
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
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(345, 34);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(98, 15);
            this.label5.TabIndex = 15;
            this.label5.Text = "Room Number : ";
            // 
            // textBox_roomNumber
            // 
            this.textBox_roomNumber.Location = new System.Drawing.Point(449, 31);
            this.textBox_roomNumber.Name = "textBox_roomNumber";
            this.textBox_roomNumber.ReadOnly = true;
            this.textBox_roomNumber.Size = new System.Drawing.Size(49, 23);
            this.textBox_roomNumber.TabIndex = 14;
            this.textBox_roomNumber.TabStop = false;
            // 
            // textBox_roomTitle
            // 
            this.textBox_roomTitle.Location = new System.Drawing.Point(96, 31);
            this.textBox_roomTitle.Name = "textBox_roomTitle";
            this.textBox_roomTitle.ReadOnly = true;
            this.textBox_roomTitle.Size = new System.Drawing.Size(233, 23);
            this.textBox_roomTitle.TabIndex = 13;
            this.textBox_roomTitle.TabStop = false;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(54, 35);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(40, 15);
            this.label4.TabIndex = 12;
            this.label4.Text = "Title : ";
            // 
            // Lobby
            // 
            this.Lobby.Controls.Add(this.listView_room);
            this.Lobby.Controls.Add(this.button_RoomCreate);
            this.Lobby.Controls.Add(this.button_RoomLeave);
            this.Lobby.Controls.Add(this.button_RoomEnter);
            this.Lobby.Location = new System.Drawing.Point(212, 55);
            this.Lobby.Name = "Lobby";
            this.Lobby.Size = new System.Drawing.Size(369, 160);
            this.Lobby.TabIndex = 15;
            this.Lobby.TabStop = false;
            this.Lobby.Text = "Lobby";
            // 
            // listView_room
            // 
            this.listView_room.HideSelection = false;
            this.listView_room.Location = new System.Drawing.Point(7, 22);
            this.listView_room.MultiSelect = false;
            this.listView_room.Name = "listView_room";
            this.listView_room.Size = new System.Drawing.Size(250, 125);
            this.listView_room.TabIndex = 16;
            this.listView_room.UseCompatibleStateImageBehavior = false;
            this.listView_room.ColumnWidthChanging += new System.Windows.Forms.ColumnWidthChangingEventHandler(this.listView_room_ColumnWidthChanging);
            // 
            // button_RoomCreate
            // 
            this.button_RoomCreate.Enabled = false;
            this.button_RoomCreate.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.button_RoomCreate.Location = new System.Drawing.Point(268, 22);
            this.button_RoomCreate.Name = "button_RoomCreate";
            this.button_RoomCreate.Size = new System.Drawing.Size(91, 33);
            this.button_RoomCreate.TabIndex = 15;
            this.button_RoomCreate.Text = "Create";
            this.button_RoomCreate.UseVisualStyleBackColor = true;
            this.button_RoomCreate.UseWaitCursor = true;
            this.button_RoomCreate.Click += new System.EventHandler(this.button_RoomCreate_Click);
            // 
            // button_RoomLeave
            // 
            this.button_RoomLeave.Enabled = false;
            this.button_RoomLeave.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.button_RoomLeave.Location = new System.Drawing.Point(268, 113);
            this.button_RoomLeave.Name = "button_RoomLeave";
            this.button_RoomLeave.Size = new System.Drawing.Size(91, 33);
            this.button_RoomLeave.TabIndex = 14;
            this.button_RoomLeave.Text = "Leave";
            this.button_RoomLeave.UseVisualStyleBackColor = true;
            this.button_RoomLeave.Click += new System.EventHandler(this.button_RoomLeave_Click);
            // 
            // button_RoomEnter
            // 
            this.button_RoomEnter.Enabled = false;
            this.button_RoomEnter.Font = new System.Drawing.Font("맑은 고딕", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point);
            this.button_RoomEnter.Location = new System.Drawing.Point(268, 69);
            this.button_RoomEnter.Name = "button_RoomEnter";
            this.button_RoomEnter.Size = new System.Drawing.Size(91, 33);
            this.button_RoomEnter.TabIndex = 10;
            this.button_RoomEnter.Text = "Enter";
            this.button_RoomEnter.UseVisualStyleBackColor = true;
            this.button_RoomEnter.Click += new System.EventHandler(this.button_RoomEnter_Click);
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(594, 699);
            this.Controls.Add(this.Lobby);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.label_Status);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.button_isConnect);
            this.Controls.Add(this.textBox_port);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.textBox_IP);
            this.Controls.Add(this.label1);
            this.Name = "MainForm";
            this.Text = "Form1";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.mainForm_FormClosing);
            this.Load += new System.EventHandler(this.mainForm_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.Lobby.ResumeLayout(false);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        private void ListBox_chat_DrawItem(object sender, System.Windows.Forms.DrawItemEventArgs e)
        {
            throw new System.NotImplementedException();
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
        private System.Windows.Forms.Label label_Status;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox Lobby;
        private System.Windows.Forms.Button button_RoomEnter;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox textBox_roomNumber;
        private System.Windows.Forms.TextBox textBox_roomTitle;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Button button_RoomCreate;
        private System.Windows.Forms.Button button_RoomLeave;
        private ListView listView_room;
        private Button button_register;
    }
}

