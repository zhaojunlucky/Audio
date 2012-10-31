using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.Security.Principal;
using System.Diagnostics;
using System.IO;

namespace Uninstall
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            if (!IsRunAsAdmin())
            {
                // Launch itself as administrator
                ProcessStartInfo proc = new ProcessStartInfo();
                proc.UseShellExecute = true;
                proc.WorkingDirectory = Environment.CurrentDirectory;
                proc.FileName = Application.ExecutablePath;
                proc.Verb = "runas";

                try
                {
                    Process.Start(proc);
                }
                catch
                {
                    // The user refused to allow privileges elevation.
                    // Do nothing and return directly ...
                    this.Close();  // Quit itself
                    return;
                }

                this.Close();  // Quit itself
                return;
            }
            string path = AppDomain.CurrentDomain.BaseDirectory;
            string para = string.Format("ZJUNINSATLL{0}", path.Replace(" ", "?"));
            string dirpath = System.IO.Path.GetTempPath() + "ZLPInst\\";
            if (!Directory.Exists(dirpath))
                Directory.CreateDirectory(dirpath);
            string exepath = dirpath + "uninstall.exe";
            try
            {
                byte[] buffer = Properties.Resources.ZLPUnInstall;
                FileStream FS = new FileStream(exepath, FileMode.Create);//新建文件 
                BinaryWriter BWriter = new BinaryWriter(FS);//以二进制打开文件流 
                BWriter.Write(buffer, 0, buffer.Length);//从资源文件读取文件内容，写入到一个文件中 
                BWriter.Close();
                FS.Close();
                Process p = Process.GetCurrentProcess();
                
                System.Diagnostics.Process.Start(exepath, para);
                p.PriorityClass = ProcessPriorityClass.RealTime;
                this.Close();
            }
            catch
            {
            }
            this.Close();  // Quit itself
            return;
            //InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {

        }

        internal bool IsRunAsAdmin()
        {
            WindowsIdentity id = WindowsIdentity.GetCurrent();
            WindowsPrincipal principal = new WindowsPrincipal(id);
            return principal.IsInRole(WindowsBuiltInRole.Administrator);
        }
    }
}
