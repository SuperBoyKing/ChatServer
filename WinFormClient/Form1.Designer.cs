
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
            this.TextBox_IP = new System.Windows.Forms.TextBox();
            this.TextBox_port = new System.Windows.Forms.TextBox();
            this.label2 = new System.Windows.Forms.Label();
            this.Button_isConnect = new System.Windows.Forms.Button();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.Button_login = new System.Windows.Forms.Button();
            this.TextBox_password = new System.Windows.Forms.TextBox();
            this.pw = new System.Windows.Forms.Label();
            this.TextBox_ID = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.textBox3 = new System.Windows.Forms.TextBox();
            this.ListView_user = new System.Windows.Forms.ListView();
            this.ListView_chat = new System.Windows.Forms.ListView();
            this.Button_chat = new System.Windows.Forms.Button();
            this.TextBox_chat = new System.Windows.Forms.TextBox();
            this.groupBox1.SuspendLayout();
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
            // TextBox_IP
            // 
            this.TextBox_IP.Location = new System.Drawing.Point(81, 13);
            this.TextBox_IP.Name = "TextBox_IP";
            this.TextBox_IP.Size = new System.Drawing.Size(144, 23);
            this.TextBox_IP.TabIndex = 1;
            // 
            // TextBox_port
            // 
            this.TextBox_port.Location = new System.Drawing.Point(325, 14);
            this.TextBox_port.Name = "TextBox_port";
            this.TextBox_port.Size = new System.Drawing.Size(81, 23);
            this.TextBox_port.TabIndex = 3;
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
            // Button_isConnect
            // 
            this.Button_isConnect.Location = new System.Drawing.Point(426, 14);
            this.Button_isConnect.Name = "Button_isConnect";
            this.Button_isConnect.Size = new System.Drawing.Size(263, 23);
            this.Button_isConnect.TabIndex = 4;
            this.Button_isConnect.Text = "Connect";
            this.Button_isConnect.UseVisualStyleBackColor = true;
            this.Button_isConnect.Click += new System.EventHandler(this.Button_isConnect_Click);
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.Button_login);
            this.groupBox1.Controls.Add(this.TextBox_password);
            this.groupBox1.Controls.Add(this.pw);
            this.groupBox1.Controls.Add(this.TextBox_ID);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Location = new System.Drawing.Point(15, 62);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(183, 134);
            this.groupBox1.TabIndex = 5;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Login";
            // 
            // Button_login
            // 
            this.Button_login.Location = new System.Drawing.Point(6, 99);
            this.Button_login.Name = "Button_login";
            this.Button_login.Size = new System.Drawing.Size(168, 25);
            this.Button_login.TabIndex = 6;
            this.Button_login.Text = "Login";
            this.Button_login.UseVisualStyleBackColor = true;
            this.Button_login.Click += new System.EventHandler(this.Button_login_Click);
            // 
            // TextBox_password
            // 
            this.TextBox_password.Location = new System.Drawing.Point(42, 59);
            this.TextBox_password.Name = "TextBox_password";
            this.TextBox_password.Size = new System.Drawing.Size(132, 23);
            this.TextBox_password.TabIndex = 9;
            // 
            // pw
            // 
            this.pw.AutoSize = true;
            this.pw.Location = new System.Drawing.Point(6, 62);
            this.pw.Name = "pw";
            this.pw.Size = new System.Drawing.Size(32, 15);
            this.pw.TabIndex = 8;
            this.pw.Text = "PW :";
            // 
            // TextBox_ID
            // 
            this.TextBox_ID.Location = new System.Drawing.Point(42, 22);
            this.TextBox_ID.Name = "TextBox_ID";
            this.TextBox_ID.Size = new System.Drawing.Size(132, 23);
            this.TextBox_ID.TabIndex = 7;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(6, 25);
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
            // ListView_user
            // 
            this.ListView_user.HideSelection = false;
            this.ListView_user.Location = new System.Drawing.Point(13, 226);
            this.ListView_user.Name = "ListView_user";
            this.ListView_user.Size = new System.Drawing.Size(185, 351);
            this.ListView_user.TabIndex = 6;
            this.ListView_user.UseCompatibleStateImageBehavior = false;
            // 
            // ListView_chat
            // 
            this.ListView_chat.HideSelection = false;
            this.ListView_chat.Location = new System.Drawing.Point(204, 226);
            this.ListView_chat.Name = "ListView_chat";
            this.ListView_chat.Size = new System.Drawing.Size(485, 322);
            this.ListView_chat.TabIndex = 7;
            this.ListView_chat.UseCompatibleStateImageBehavior = false;
            // 
            // Button_chat
            // 
            this.Button_chat.Location = new System.Drawing.Point(623, 554);
            this.Button_chat.Name = "Button_chat";
            this.Button_chat.Size = new System.Drawing.Size(66, 24);
            this.Button_chat.TabIndex = 8;
            this.Button_chat.Text = "chat";
            this.Button_chat.UseVisualStyleBackColor = true;
            this.Button_chat.Click += new System.EventHandler(this.Button_chat_Click);
            // 
            // TextBox_chat
            // 
            this.TextBox_chat.Location = new System.Drawing.Point(204, 555);
            this.TextBox_chat.Name = "TextBox_chat";
            this.TextBox_chat.Size = new System.Drawing.Size(413, 23);
            this.TextBox_chat.TabIndex = 9;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(701, 588);
            this.Controls.Add(this.TextBox_chat);
            this.Controls.Add(this.Button_chat);
            this.Controls.Add(this.ListView_chat);
            this.Controls.Add(this.ListView_user);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.Button_isConnect);
            this.Controls.Add(this.TextBox_port);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.TextBox_IP);
            this.Controls.Add(this.label1);
            this.Name = "Form1";
            this.Text = "Form1";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox TextBox_IP;
        private System.Windows.Forms.TextBox TextBox_port;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Button Button_isConnect;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox TextBox_password;
        private System.Windows.Forms.Label pw;
        private System.Windows.Forms.TextBox TextBox_ID;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox textBox3;
        private System.Windows.Forms.Button Button_login;
        private System.Windows.Forms.ListView ListView_user;
        private System.Windows.Forms.ListView ListView_chat;
        private System.Windows.Forms.Button Button_chat;
        private System.Windows.Forms.TextBox TextBox_chat;
    }
}

