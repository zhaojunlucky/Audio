using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using ZLPUnInstall.Properties;
using Microsoft.Win32;
using System.Threading;
using System.Diagnostics;
using System.Security.Principal;

namespace ZLPUnInstall
{
    public partial class Form1 : Form
    {
        private string appKey;
        private string installDir;
        private string startMenu;
        private string destopMenu;
        private string unregistfile;
        private bool userCancel = false;
        private bool uninstalling = false;
        private static Semaphore semaphore;
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
                    return;
                }

                this.Close();  // Quit itself
                return;
            }
            InitializeComponent();
            string[] para = Environment.GetCommandLineArgs();
            appKey = "ZLPlayer";
            string tag = "ZJUNINSATLL";
            foreach (string o in para)
            {
                if (o.StartsWith(tag))
                {
                    
                    installDir = o.Replace("?"," ").Substring(tag.Length, o.Length - tag.Length);

                    break;
                }
            }
            if (string.IsNullOrEmpty(installDir) || !Directory.Exists(installDir))
            {
                Environment.Exit(0);
                return;
            }
            startMenu = Path.Combine(Path.Combine(Settings.userStartMenu, Settings.company), Settings.appName);
            destopMenu = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Desktop),Settings.appName+".lnk");
            unregistfile = "/regflac,ape,aac,tta,mp3,wav,tak,cue,ofr,wv,m4a,ogg,";

            Readdy rd = new Readdy();
            rd.uninstall.Click += new EventHandler(uninstall_Click);
            rd.cancel.Click += new EventHandler(cancel_Click);
            this.stepPanel.Controls.Add(rd);
            
        }

        internal bool IsRunAsAdmin()
        {
            WindowsIdentity id = WindowsIdentity.GetCurrent();
            WindowsPrincipal principal = new WindowsPrincipal(id);
            return principal.IsInRole(WindowsBuiltInRole.Administrator);
        }

        private void cancel_Click(object sender, EventArgs e)
        {
            if (DialogResult.Yes == MessageBox.Show("Are you sure to exist uninstall?", "ZLPlayer uninstall", MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
            {
                userCancel = true;
                this.Close();
            }
        }

        private void uninstall_Click(object sender, EventArgs e)
        {
            Readdy rd = (Readdy)this.stepPanel.Controls[0];
            rd.uninstall.Click -= new EventHandler(uninstall_Click); ;
            rd.cancel.Click -= new EventHandler(cancel_Click);
            bool canCreateNew;
            Mutex m = new Mutex(true, Settings.mutexName, out canCreateNew);
            if (!canCreateNew)
            {
                
                CloseApp ca = new CloseApp();
                ca.richTextBox1.AppendText("ZLPlayer.exe");
                ca.continueBtn.Click += new EventHandler(continueBtn_Click);
                this.stepPanel.Controls.RemoveAt(0);
                this.stepPanel.Controls.Add(ca);
                rd.Dispose();
                return;
            }
            m.ReleaseMutex();
            UnInstalling ui = new UnInstalling();
            this.stepPanel.Controls.RemoveAt(0);
            this.stepPanel.Controls.Add(ui);

            rd.Dispose();
            uninstalling = true;
            backgroundWorker1.RunWorkerAsync();
        }

        private void continueBtn_Click(object sender, EventArgs e)
        {
            CloseApp ca = (CloseApp)this.stepPanel.Controls[0];
            ca.continueBtn.Enabled = false;
            ca.richTextBox1.Text = "Closing Program...";

            semaphore = new Semaphore(0, 1);
            Thread thread = new Thread(ThreadFun);
            thread.Start();
            semaphore.WaitOne();
            semaphore.Close();
            ca.continueBtn.Click -= new EventHandler(continueBtn_Click);

            UnInstalling ui = new UnInstalling();
            this.stepPanel.Controls.RemoveAt(0);
            this.stepPanel.Controls.Add(ui);
            ca.Dispose();
            uninstalling = true;
            backgroundWorker1.RunWorkerAsync();
        }

        private void ThreadFun()
        {
            Process[] ps = Process.GetProcesses();
            foreach (Process p in ps)
            {
                string path = Path.Combine(installDir, Settings.appExeName).ToLower();
                try
                {
                    if (p.MainModule.FileName.ToLower() == path)
                        p.Kill();
                }
                catch
                {

                }
            }
            semaphore.Release();
        }

        private void deleteShortcuts()
        {
            if (Directory.Exists(startMenu))
                Directory.Delete(startMenu,true);
            if (File.Exists(destopMenu))
                File.Delete(destopMenu);
        }
        private void deleteDir()
        {
            if(Directory.Exists(installDir))
                Directory.Delete(installDir,true);
        }
        private void unreigstfile()
        {
            Process p = new Process();
            
            p.StartInfo.FileName = Path.Combine(installDir, Settings.appExeName);//System.Diagnostics.Process.Start(, unregistfile);
            p.StartInfo.Arguments = unregistfile;
            p.StartInfo.CreateNoWindow = true;
            p.Start();
            try
            {
                p.PriorityClass = ProcessPriorityClass.RealTime;
            }
            catch
            {
            }
        }

        private void deleteUninsatllinfo(bool x64 = false)
        {
            RegistryKey hKey = Registry.LocalMachine;
            string subKey;
            //if (x64)
                subKey = @"SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\" ;
            //else
               // subKey = @"SOFTWARE\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\";
            RegistryKey uninsKey = hKey.OpenSubKey(subKey,true);
            uninsKey.DeleteSubKey(appKey);
            uninsKey.Close();
            hKey.Close();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (userCancel && !uninstalling)
                return;
            if (uninstalling)
            {
                e.Cancel = true;
                return;
            }
            if (DialogResult.No == MessageBox.Show("Are you sure to exist uninstall?", "ZLPlayer uninstall", MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
            {
                e.Cancel = true;
            }
        }

        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {
            backgroundWorker1.ReportProgress(0);
            unreigstfile(); 
            Thread.Sleep(500);
            backgroundWorker1.ReportProgress(25);
            deleteUninsatllinfo();
            //Thread.Sleep(1000);
            backgroundWorker1.ReportProgress(50);
            deleteShortcuts();
            //Thread.Sleep(1000);
            backgroundWorker1.ReportProgress(75);
            deleteDir();
            //Thread.Sleep(1000);
            backgroundWorker1.ReportProgress(100);
        }

        private void backgroundWorker1_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            UnInstalling ui = (UnInstalling)this.stepPanel.Controls[0];
            if (0 == e.ProgressPercentage)
            {
                ui.status.Text = "Unregisting file association...";
            }
            else if (25 == e.ProgressPercentage)
            {
                ui.status.Text = "Deleting Registry...";
            }
            else if (75 == e.ProgressPercentage)
            {
                ui.status.Text = "Deleting shortcuts..";
            }
            ui.progressBar1.Value = e.ProgressPercentage;
        }

        private void backgroundWorker1_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            UnInstalling ui = (UnInstalling)this.stepPanel.Controls[0];
            ui.status.Text = "deleting program folder...";
            ui.progressBar1.Value = 100;
            Thread.Sleep(300);
            uninstalling = false;
            Finish fi = new Finish();
            fi.finishBtn.Click += new EventHandler(finishBtn_Click);
            this.stepPanel.Controls.RemoveAt(0);
            this.stepPanel.Controls.Add(fi);
            ui.Dispose();
        }

        private void finishBtn_Click(object sender, EventArgs e)
        {
            userCancel = true;
            this.Close();
        }

        private void Form1_FormClosed(object sender, FormClosedEventArgs e)
        {

            string filename = Path.Combine(AppDomain.CurrentDomain.BaseDirectory, "killmyself.bat");
            using (StreamWriter bat = File.CreateText(filename))
            {
                // 自删除
                  bat.WriteLine(string.Format(@"
                  @echo off
                  :selfkill
                  attrib -a -r -s -h ""{0}""
                  del ""{0}""
                  if exist ""{0}"" goto selfkill
                  del %0 ", AppDomain.CurrentDomain.FriendlyName));

            }
            // 启动自删除批处理文件
            ProcessStartInfo info = new ProcessStartInfo(filename);
            info.WindowStyle = ProcessWindowStyle.Hidden;
            Process.Start(info);
            // 强制关闭当前进程
            Environment.Exit(0);
        }
    }
}
