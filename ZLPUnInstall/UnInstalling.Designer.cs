namespace ZLPUnInstall
{
    partial class UnInstalling
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
            this.label2 = new System.Windows.Forms.Label();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.status = new System.Windows.Forms.Label();
            this.SuspendLayout();
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.BackColor = System.Drawing.SystemColors.Control;
            this.label2.Font = new System.Drawing.Font("Times New Roman", 14.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.label2.ForeColor = System.Drawing.Color.Fuchsia;
            this.label2.Location = new System.Drawing.Point(3, 22);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(169, 21);
            this.label2.TabIndex = 17;
            this.label2.Text = "Uninstalling ZLPlayer";
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(78, 198);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(339, 19);
            this.progressBar1.TabIndex = 19;
            // 
            // status
            // 
            this.status.AutoSize = true;
            this.status.BackColor = System.Drawing.SystemColors.Control;
            this.status.Font = new System.Drawing.Font("Times New Roman", 12F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.status.ForeColor = System.Drawing.Color.Fuchsia;
            this.status.Location = new System.Drawing.Point(3, 67);
            this.status.Name = "status";
            this.status.Size = new System.Drawing.Size(137, 19);
            this.status.TabIndex = 18;
            this.status.Text = "Uninstalling ZLPlayer";
            // 
            // UnInstalling
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.progressBar1);
            this.Controls.Add(this.status);
            this.Controls.Add(this.label2);
            this.Name = "UnInstalling";
            this.Size = new System.Drawing.Size(514, 350);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Label label2;
        public System.Windows.Forms.ProgressBar progressBar1;
        public System.Windows.Forms.Label status;
    }
}
