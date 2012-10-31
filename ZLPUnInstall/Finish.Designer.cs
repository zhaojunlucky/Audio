namespace ZLPUnInstall
{
    partial class Finish
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
            this.finishBtn = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // status
            // 
            this.status.AutoSize = true;
            this.status.BackColor = System.Drawing.SystemColors.Control;
            this.status.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.status.ForeColor = System.Drawing.Color.Fuchsia;
            this.status.Location = new System.Drawing.Point(16, 35);
            this.status.Name = "status";
            this.status.Size = new System.Drawing.Size(179, 19);
            this.status.TabIndex = 19;
            this.status.Text = "Uninstall ZLPlayer complete";
            // 
            // finishBtn
            // 
            this.finishBtn.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.finishBtn.Location = new System.Drawing.Point(204, 272);
            this.finishBtn.Name = "finishBtn";
            this.finishBtn.Size = new System.Drawing.Size(80, 39);
            this.finishBtn.TabIndex = 20;
            this.finishBtn.Text = "Finish";
            this.finishBtn.UseVisualStyleBackColor = true;
            // 
            // Finish
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.finishBtn);
            this.Controls.Add(this.status);
            this.Name = "Finish";
            this.Size = new System.Drawing.Size(514, 350);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label status;
        public System.Windows.Forms.Button finishBtn;
    }
}
