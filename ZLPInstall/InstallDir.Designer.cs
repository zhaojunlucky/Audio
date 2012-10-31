namespace ZLPInstall
{
    partial class InstallDir
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
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

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.status = new System.Windows.Forms.Label();
            this.pictureBox1 = new System.Windows.Forms.PictureBox();
            this.dir = new System.Windows.Forms.Label();
            this.install = new System.Windows.Forms.Button();
            this.custom = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).BeginInit();
            this.SuspendLayout();
            // 
            // status
            // 
            this.status.AutoSize = true;
            this.status.BackColor = System.Drawing.Color.Transparent;
            this.status.Font = new System.Drawing.Font("Calibri", 15.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.status.ForeColor = System.Drawing.Color.Fuchsia;
            this.status.Location = new System.Drawing.Point(32, 109);
            this.status.Name = "status";
            this.status.Size = new System.Drawing.Size(474, 26);
            this.status.TabIndex = 3;
            this.status.Text = "Application will be installed to the following directory ";
            // 
            // pictureBox1
            // 
            this.pictureBox1.Image = global::ZLPInstall.Properties.Resources.ZJA;
            this.pictureBox1.Location = new System.Drawing.Point(3, 3);
            this.pictureBox1.Name = "pictureBox1";
            this.pictureBox1.Size = new System.Drawing.Size(64, 64);
            this.pictureBox1.SizeMode = System.Windows.Forms.PictureBoxSizeMode.StretchImage;
            this.pictureBox1.TabIndex = 4;
            this.pictureBox1.TabStop = false;
            // 
            // dir
            // 
            this.dir.AutoSize = true;
            this.dir.BackColor = System.Drawing.Color.Transparent;
            this.dir.Font = new System.Drawing.Font("Calibri", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.dir.ForeColor = System.Drawing.Color.Fuchsia;
            this.dir.Location = new System.Drawing.Point(46, 164);
            this.dir.Name = "dir";
            this.dir.Size = new System.Drawing.Size(420, 23);
            this.dir.TabIndex = 5;
            this.dir.Text = "Application will be installed to the following directory ";
            this.dir.TextChanged += new System.EventHandler(this.dir_TextChanged);
            // 
            // install
            // 
            this.install.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.install.Location = new System.Drawing.Point(183, 225);
            this.install.Name = "install";
            this.install.Size = new System.Drawing.Size(126, 46);
            this.install.TabIndex = 6;
            this.install.Text = "Install Now";
            this.install.UseCompatibleTextRendering = true;
            this.install.UseVisualStyleBackColor = true;
            // 
            // custom
            // 
            this.custom.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.custom.Location = new System.Drawing.Point(209, 294);
            this.custom.Name = "custom";
            this.custom.Size = new System.Drawing.Size(78, 37);
            this.custom.TabIndex = 7;
            this.custom.Text = "Custom";
            this.custom.UseCompatibleTextRendering = true;
            this.custom.UseVisualStyleBackColor = true;
            // 
            // InstallDir
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.custom);
            this.Controls.Add(this.install);
            this.Controls.Add(this.dir);
            this.Controls.Add(this.pictureBox1);
            this.Controls.Add(this.status);
            this.Name = "InstallDir";
            this.Size = new System.Drawing.Size(514, 352);
            ((System.ComponentModel.ISupportInitialize)(this.pictureBox1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        public System.Windows.Forms.Label status;
        private System.Windows.Forms.PictureBox pictureBox1;
        public System.Windows.Forms.Label dir;
        public System.Windows.Forms.Button install;
        public System.Windows.Forms.Button custom;

    }
}
