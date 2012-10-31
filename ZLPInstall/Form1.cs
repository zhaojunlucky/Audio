using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using ICSharpCode.SharpZipLib.Zip;
using System.Runtime.InteropServices;
using System.IO;
using IWshRuntimeLibrary;
using System.Diagnostics;
using System.Security.Principal;
using System.Threading;

namespace ZLPInstall
{
    public partial class Form1 : Form
    {
        [StructLayout(LayoutKind.Sequential)]
        public struct MARGINS
        {
            public int Left;
            public int Right;
            public int Top;
            public int Bottom;
        }

        [DllImport("dwmapi.dll", PreserveSig = false)]
        static extern void DwmExtendFrameIntoClientArea(IntPtr hwnd, ref MARGINS margins);

        [DllImport("dwmapi.dll", PreserveSig = false)]
        static extern bool DwmIsCompositionEnabled();

        [DllImport("User32.dll", EntryPoint = "SendMessage")]

        private static extern int SendMessage(int hWnd, int Msg, int wParam, int lParam);

        [DllImport("uxtheme.dll")]
        public static extern int SetWindowTheme([In] IntPtr hwnd,[In, MarshalAs(UnmanagedType.LPWStr)] string pszSubAppName,
            [In, MarshalAs(UnmanagedType.LPWStr)] string pszSubIdList);


        [DllImport("Gdi32", EntryPoint = "AddFontResource")]
        private static extern int AddFontResource(string fontSource);
        [DllImport("gdi32")]
        static extern bool CreateScalableFontResource(uint fdwHidden, string lpszFontRes, string lpszFontFile, string lpszCurrentPath);

        [DllImport("kernel32.dll", SetLastError = true)]
        static extern int WriteProfileString(string lpszSection, string lpszKeyName, string lpszString);

        [DllImport("kernel32")]
        private static extern int GetPrivateProfileString(string section, string key, string def, StringBuilder retVal, int size, string filePath);

        public const int USER_STEP1_BEGIN_DE_ZIP = 0x0401; // 是windows系统定义的用户消息 
        //public const int USER_STEP1_BEGIN_DE_ZIP = 0x0401; // 是windows系统定义的用户消息
        private bool custom = false;
        private long tsize = 0;
        private string dir = "";
        private InstallStatue inStatus = InstallStatue.ZJReady;
        private static long fileCopySize = 0;
        private string tmpDir;
        private delegate void BGWorkReport(int process);
        private BGWorkReport bgWorker;
        private bool userCanel = false;
        private static Semaphore semaphore;
        public static Color c = Color.FromArgb(255, 221, 220, 220);
        enum InstallStatue
        {
            ZJReady,
            ZJInstalling,
            ZJInstalled
        }
        public Form1()
        {
            //MessageBox.Show(Environment.Is64BitOperatingSystem.ToString());
            // check os 
            Version v = System.Environment.OSVersion.Version;
            double ver = v.Major + v.Minor / 10.0;

            
            if (ver <= 6.0)
            {
                MessageBox.Show("This software only run on Windows 7 or later!!!");
                Environment.Exit(0);
                return;
            }

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
                catch(Exception ex)
                {
                    // The user refused to allow privileges elevation.
                    // Do nothing and return directly ...
                    
                }

                Environment.Exit(0);  // Quit itself
                return;
            }
            InitializeComponent();
            if (DwmIsCompositionEnabled())
            {
                MARGINS m = new MARGINS();
                m.Right = m.Bottom = -1;
                m.Left = m.Top = -1;// this.Width + this.Height;
                DwmExtendFrameIntoClientArea(this.Handle, ref m);
            }
            bgWorker = new BGWorkReport(ReportProcess);

            backgroundWorker1.RunWorkerAsync();
            this.BringToFront();
        }

        internal bool IsRunAsAdmin()
        {
            WindowsIdentity id = WindowsIdentity.GetCurrent();
            WindowsPrincipal principal = new WindowsPrincipal(id);
            return principal.IsInRole(WindowsBuiltInRole.Administrator);
        }

        private void SetWindowsTheme(IntPtr handle, string theme = "Explorer")
        {
            SetWindowTheme(handle, theme, null);
        }
        private void Form1_Load(object sender, EventArgs e)
        {

            
            this.TransparencyKey = c;
            this.stepPanel.BackColor = c;

            this.Icon = Properties.Resources.ZLPlayer;
            Ready r = new Ready();
            r.BackColor = c;
            //r.aeroLabel1.PControl = this;
            this.stepPanel.Dock = DockStyle.Fill;
            this.stepPanel.Controls.Add(r);
        }

        private void ReportProcess(int process)
        {
            //backgroundWorker1.ReportProgress(process>0?process:0);     
            ((Installing)this.stepPanel.Controls[0]).progressBar1.Value = process;
        }

        private void backgroundWorker1_DoWork(object sender, DoWorkEventArgs e)
        {
            string dirpath = System.IO.Path.GetTempPath() + "ZLPInst\\";
            if (!Directory.Exists(dirpath))
                Directory.CreateDirectory(dirpath);
            string zippath = dirpath + "zji.zip";
            byte[] buffer = Properties.Resources.zip;//这个是添加EXE到程序资源时的名称 
            FileStream FS = new FileStream(zippath, FileMode.Create);//新建文件 
            BinaryWriter BWriter = new BinaryWriter(FS);//以二进制打开文件流 
            BWriter.Write(buffer, 0, buffer.Length);//从资源文件读取文件内容，写入到一个文件中 
            BWriter.Close();
            FS.Close();
         
            
            using (ZipFile zFile = new ZipFile(zippath))
            {
                foreach (ZipEntry z in zFile)
                {
                    if (z.IsFile)
                    {
                        tsize += z.Size;
                    }
                }
            }
            string fontDir = Path.Combine(Environment.GetFolderPath(System.Environment.SpecialFolder.Windows),"fonts\\");
            bool hasFont = System.IO.File.Exists(Path.Combine(fontDir, "zh.ttf")) ;
         
  
            backgroundWorker1.ReportProgress(0);
            string dir = dirpath + "ZLPlayer\\";
            tmpDir = dir;
            long deSize = 0;
            ZipInputStream s = new ZipInputStream(System.IO.File.OpenRead(zippath));
            try
            {
                if (!System.IO.File.Exists(zippath))
                    return;
                dir = dir.Replace("/", "\\");
                if (!dir.EndsWith("\\"))
                    dir += "\\";
                if (!Directory.Exists(dir))
                    Directory.CreateDirectory(dir);

                ZipEntry theEntry;
                while ((theEntry = s.GetNextEntry()) != null)
                {
                    string directoryName = Path.GetDirectoryName(theEntry.Name);
                    string fileName = Path.GetFileName(theEntry.Name);

                    if (directoryName != String.Empty)
                        Directory.CreateDirectory(dir + directoryName);
                    if (fileName != String.Empty)
                    {
                        FileStream streamWriter = System.IO.File.Create(dir + theEntry.Name);
                        int size = 2048;
                        byte[] data = new byte[2048];
                        while (true)
                        {
                            size = s.Read(data, 0, data.Length);
                            if (size > 0)
                            {
                                streamWriter.Write(data, 0, size);
                                deSize += size;
                                backgroundWorker1.ReportProgress((int)(deSize * 1000 / tsize / 10));
                            }
                            else
                            {
                                break;
                            }
                        }
                        streamWriter.Close();
                    }
                }
                if (!hasFont)
                {
                    FS = new FileStream(dirpath + "zh.ttf", FileMode.Create);
                    BWriter = new BinaryWriter(FS);//以二进制打开文件流 
                    BWriter.Write(Properties.Resources.zh, 0, Properties.Resources.zh.Length);//从资源文件读取文件内容，写入到一个文件中 
                    BWriter.Close();
                    FS.Close();
                    System.IO.File.Copy(dirpath + "zh.ttf", fontDir + "zh.ttf");
                    const int WM_FONTCHANGE = 0x001D;
                    const int HWND_BROADCAST = 0xffff;               

                    int r = AddFontResource(fontDir + "zh.ttf");




                    if (AddFontResource(fontDir + "zh.ttf") <= 0)
                    {
                        System.IO.File.Delete(fontDir + "zh.ttf");
                        MessageBox.Show("The system can't install the font automaticly!Please Press install button,otherwise,the lyric function will be disabled!!!");
                        System.Diagnostics.Process.Start(dirpath + "zh.ttf");
                    }
                    else
                    {
                        WriteProfileString("fonts", "FZMM (TrueType)", "zh.ttf");
                        SendMessage(HWND_BROADCAST, WM_FONTCHANGE, 0, 0);
                    }
                    System.IO.File.Delete(dirpath + "zh.ttf");
                }
                
                if(deSize < tsize)
                    backgroundWorker1.ReportProgress(100);
              
            }
            catch (Exception)
            {

            }
            finally
            {
                s.Close();
            }


        }

        private void backgroundWorker1_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            if (0 == e.ProgressPercentage)
                ((Ready)this.stepPanel.Controls[0]).progressBar1.Style = ProgressBarStyle.Continuous;
            else
                ((Ready)this.stepPanel.Controls[0]).progressBar1.Value = e.ProgressPercentage;
        }

        private void backgroundWorker1_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            // step2
            ((Ready)this.stepPanel.Controls[0]).progressBar1.Value = 100;
            Agreement am = new Agreement();
         
            
            SetWindowsTheme(am.richTextBox1.Handle);
            //SetWindowsTheme(am.label1.Handle);
            am.richTextBox1.AppendText(  Properties.Resources.agreement);
            am.accept.Click += new EventHandler(accept_Click);
            am.cancel.Click += new EventHandler(cancel_Click);
            UserControl uc = (UserControl)this.stepPanel.Controls[0];
            this.stepPanel.Controls.RemoveAt(0);
            this.stepPanel.Controls.Add(am);
            uc.Dispose();

            
            //GlassText gt = new GlassText();
            //gt.FillBlackRegion(am.CreateGraphics(), am.ClientRectangle);
            //gt.DrawTextOnGlass(am.Handle, am.label1.Text, am.label1.Font, am.label1.Bounds, 10);
        }

        private void cancel_Click(object sender, EventArgs e)
        {
            if (DialogResult.Yes == MessageBox.Show("Are you sure to exist ZLPlayer setup?", "ZLPlayer setup", MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
            {
                userCanel = true;
                this.Close();
            }
        }

        private void accept_Click(object sender, EventArgs e)
        {
            Agreement am = (Agreement)this.stepPanel.Controls[0];
            InstallDir id = new InstallDir();
            id.install.Click += new EventHandler(install_Click);
            id.custom.Click += new EventHandler(custom_Click);
            dir = RegistryHelper.FindPrevioudInstallDir(Settings.appKey/*"{485D8730-5233-4F7F-832C-84D142B5C1FA}"*/, Environment.Is64BitOperatingSystem);
            if (string.IsNullOrEmpty(dir))
            {
                dir = RegistryHelper.GetDefaultProgramDir(Environment.Is64BitOperatingSystem);
                dir = Path.Combine(Path.Combine(dir, Settings.company), Settings.appName);
            }
            if (string.IsNullOrEmpty(dir))
            {
                dir = @"C:\Program files\";
                dir = Path.Combine(Path.Combine(dir, Settings.company), Settings.appName);
            }
            //dir += @"ZJSoft\ZLPlayer\";
            
            if (!dir.EndsWith("\\"))
                dir += "\\";
            id.dir.Text = dir;
            
            this.stepPanel.Controls.RemoveAt(0);
            this.stepPanel.Controls.Add(id);
            am.accept.Click -= new EventHandler(accept_Click);
            am.cancel.Click -= new EventHandler(cancel_Click);
            am.Dispose();
        }

        private void custom_Click(object sender, EventArgs e)
        {
            custom = true;
            InstallDir id = (InstallDir)this.stepPanel.Controls[0];
            id.install.Click -= new EventHandler(install_Click);
            id.custom.Click -= new EventHandler(custom_Click);
            Options op = new Options();
    
            //op.status.PControl = op;
            op.install.Click += new EventHandler(install_Click);
            op.dir.Text = dir;
            string drivePath = dir.Substring(0, 3);
            DriveInfo info = new DriveInfo(drivePath);

            op.disk.Text = "Required disk space:" + GetSizeInfo(tsize) + "(Available space:"+GetSizeInfo(info.AvailableFreeSpace)+")";
            op.folder.Click += new EventHandler(folder_Click);
            op.flac.Checked = RegistryHelper.IsFileAss(".flac", "ZLP.FLAC");
            op.m4a.Checked = RegistryHelper.IsFileAss(".m4a", "ZLP.M4A");
            op.mp3.Checked = RegistryHelper.IsFileAss(".mp3", "ZLP.MP3");
            op.cue.Checked = RegistryHelper.IsFileAss(".cue", "ZLP.CUE");
            op.aac.Checked = RegistryHelper.IsFileAss(".aac", "ZLP.aac");
            op.tta.Checked = RegistryHelper.IsFileAss(".tta", "ZLP.TTA");
            op.ofr.Checked = RegistryHelper.IsFileAss(".ofr", "ZLP.OFR");
            op.tak.Checked = RegistryHelper.IsFileAss(".tak", "ZLP.TAK");
            op.wav.Checked = RegistryHelper.IsFileAss(".wav", "ZLP.WAV");
            op.wv.Checked = RegistryHelper.IsFileAss(".wv", "ZLP.WV");
            op.ogg.Checked = RegistryHelper.IsFileAss(".ogg", "ZLP.OGG");
            op.ape.Checked = RegistryHelper.IsFileAss(".ape", "ZLP.APE");
            this.stepPanel.Controls.RemoveAt(0);
            this.stepPanel.Controls.Add(op);
            id.Dispose();
        }

       

        private void folder_Click(object sender, EventArgs e)
        {
            FolderBrowserDialog f = new FolderBrowserDialog();
            f.ShowNewFolderButton = true;
            f.SelectedPath = dir;
            
            if (DialogResult.OK == f.ShowDialog())
            {
                dir = f.SelectedPath;
                Options op = (Options)this.stepPanel.Controls[0];
                op.dir.Text = dir;
                if(!dir.EndsWith("\\"))
                    dir += "\\";
                string drivePath = dir.Substring(0, 3);
                DriveInfo info = new DriveInfo(drivePath);

                op.disk.Text = "Required disk space:" + GetSizeInfo(tsize) + "(Available space:" + GetSizeInfo(info.AvailableFreeSpace) + ")";
            }
        }

        private string GetSizeInfo(long size)
        {
            if (size <= 0)
                return "0KB";
            if (size < 1024)
                return string.Format("{0}bytes", size);
            double kb = (double)size / 1024;
            if (kb < 1024)
                return string.Format("{0}KB", Math.Round(kb,2));
            double mb = kb / 1024;
            if (mb < 1024)
                return string.Format("{0}MB", Math.Round(mb,2));
            return string.Format("{0}GB", Math.Round(mb / 1024,2));
        }

        private void install_Click(object sender, EventArgs e)
        {
            //
            UserControl uc;
            if (!custom)
            {
                InstallDir id = (InstallDir)this.stepPanel.Controls[0];
                id.install.Click -= new EventHandler(install_Click);
                id.custom.Click -= new EventHandler(custom_Click);
                uc = id;
                
            }
            else
            {
                Options op = (Options)this.stepPanel.Controls[0];
                op.install.Click -= new EventHandler(install_Click);
                uc = op;
                dir = op.dir.Text;
                Settings.createDesktopshortcut = op.desktop.Checked;
                Settings.createTaskbar = op.taskbar.Checked;
                Settings.createStartMenu = op.startMenu.Checked;

                string regist = "/reg";
                string unregist = "";
                if (op.flac.Checked)
                    regist += "flac|";
                else
                    unregist += "flac,";
                if (op.aac.Checked)
                    regist += "aac|";
                else
                    unregist += "aac,";
                if (op.ape.Checked)
                    regist += "ape|";
                else
                    unregist += "ape,";
                if (op.cue.Checked)
                    regist += "cue|";
                else
                    unregist += "cue,";
                if (op.m4a.Checked)
                    regist += "m4a|";
                else
                    unregist += "m4a,";
                if (op.mp3.Checked)
                    regist += "mp3|";
                else
                    unregist += "mp3,";
                if (op.ofr.Checked)
                    regist += "ofr|";
                else
                    unregist += "ofr,";
                if (op.ogg.Checked)
                    regist += "ogg|";
                else
                    unregist += "ogg,";
                if (op.tak.Checked)
                    regist += "tak|";
                else
                    unregist += "tak,";
                if (op.tta.Checked)
                    regist += "tta|";
                else
                    unregist += "tta,";
                if (op.wav.Checked)
                    regist += "wav|";
                else
                    unregist += "wav,";
                if (op.wv.Checked)
                    regist += "wv|";
                else
                    unregist += "wv,";
                Settings.fileAss = regist + unregist;
            }
            
            bool canCreateNew;
            Mutex m = new Mutex(true, Settings.mutexName, out canCreateNew);
            if (!canCreateNew)
            {
                CloseApp ca = new CloseApp();
                ca.richTextBox1.AppendText("ZLPlayer.exe");
                ca.continueBtn.Click += new EventHandler(continueBtn_Click);
                this.stepPanel.Controls.RemoveAt(0);
                this.stepPanel.Controls.Add(ca);
                uc.Dispose();
                
                return;
            }
            m.ReleaseMutex();
            Installing iti = new Installing();
            this.stepPanel.Controls.RemoveAt(0);
            this.stepPanel.Controls.Add(iti);
            uc.Dispose();
            this.inStatus = InstallStatue.ZJInstalling;
            backgroundWorker2.RunWorkerAsync();
        }

        private void ThreadFun()
        {
            Process[] ps = Process.GetProcesses();
            foreach (Process p in ps)
            {
                string path = Path.Combine(dir, Settings.appExeName).ToLower();
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
            Thread.Sleep(1500);
            Installing iti = new Installing();

            this.stepPanel.Controls.RemoveAt(0);
            this.stepPanel.Controls.Add(iti);
            ca.Dispose();
            this.inStatus = InstallStatue.ZJInstalling;
            backgroundWorker2.RunWorkerAsync();
        }

        private void Form1_FormClosing(object sender, FormClosingEventArgs e)
        {
            if (userCanel)
                return;
            if (InstallStatue.ZJInstalling == this.inStatus )
                e.Cancel = true;
            else if ( InstallStatue.ZJReady == this.inStatus)
            {
                if (DialogResult.No == MessageBox.Show("Are you sure to exist ZLPlayer setup?", "ZLPlayer setup", MessageBoxButtons.YesNo, MessageBoxIcon.Warning))
                {
                    e.Cancel = true;
                }
            }
        }

        private void CopyDirectory(string srcdir, string desdir)
        {
            string folderName = srcdir.Substring(srcdir.LastIndexOf("\\") + 1);

            string desfolderdir = desdir + "\\" + folderName;

            if (desdir.LastIndexOf("\\") == (desdir.Length - 1))
            {
                desfolderdir = desdir + folderName;
            }
            string[] filenames = Directory.GetFileSystemEntries(srcdir);

            foreach (string file in filenames)// 遍历所有的文件和目录
            {
                if (Directory.Exists(file))// 先当作目录处理如果存在这个目录就递归Copy该目录下面的文件
                {

                    string currentdir = desfolderdir + "\\" + file.Substring(file.LastIndexOf("\\") + 1);
                    if (!Directory.Exists(currentdir))
                    {
                        Directory.CreateDirectory(currentdir);
                    }

                    CopyDirectory(file, desfolderdir);
                }

                else // 否则直接copy文件
                {
                    string srcfileName = file.Substring(file.LastIndexOf("\\") + 1);

                    srcfileName = Path.Combine(desfolderdir , srcfileName);


                    if (!Directory.Exists(desfolderdir))
                    {
                        Directory.CreateDirectory(desfolderdir);
                    }


                    //File.Copy(file, srcfileName);
                    CopyFile(file,srcfileName );
                }
            }//foreach 
        }//function end

        private void CopyFile(string srcFile, string desFile)
        {
            if (!System.IO.File.Exists(srcFile))
                return;
            if (System.IO.File.Exists(desFile))
                System.IO.File.Delete(desFile);
            FileInfo f = new FileInfo(srcFile);
            FileStream fsR = f.OpenRead();
            FileStream fsW = System.IO.File.Create(desFile);
            long fileLength = f.Length;
            byte[] buffer = new byte[1024];
            int dataRead = 1;
            while (dataRead > 0)
            {
                dataRead = fsR.Read(buffer, 0, 1024);
                if (dataRead > 0)
                {

                    fsW.Write(buffer, 0, dataRead);
                    //fsW.Flush();
                    fileCopySize += dataRead;
                    //backgroundWorker2.ReportProgress((int)(fileCopySize * 1000 / tsize / 10) -3);
                    int p = (int)(fileCopySize * 1000 / tsize / 10) - 3;
                    if(p < 0)
                        p = 0;
                    this.BeginInvoke(bgWorker, p);
                    //backgroundWorker2.ReportProgress(p);
                }
            }
            fsR.Close();
            fsW.Close(); 
        }

        private void backgroundWorker2_DoWork(object sender, DoWorkEventArgs e)
        {
            CopyDirectory(tmpDir, dir);// 97%
            // create shortcuts
            CreateShortcuts();
            string p = Path.Combine(Path.Combine(Settings.userStartMenu, Settings.company), Settings.appName);
            if (Settings.createStartMenu)
            {
                CreateUserShortcuts(p, "Help", Path.Combine(dir, "help.txt"), dir, 1, Settings.appName,
                    string.Format("{0},8", Path.Combine(dir, Settings.appExeName)));
                CreateUserShortcuts(p, "ChangeLog", Path.Combine(dir, "changes.txt"), dir, 1, Settings.appName,
                   "%SystemRoot%\\system32\\imageres.dll,-102");
            }
            else
            {
                if (Directory.Exists(p))
                    Directory.Delete(p);
            }
            backgroundWorker2.ReportProgress(98);
            // set uninstall info
            RegistryHelper.SetUnInstallInfo(Settings.appKey, dir, Settings.appExeName, "2.0.0.2", Settings.appName, Settings.company, Settings.unInstallExeName, tsize, Environment.Is64BitOperatingSystem);
            backgroundWorker2.ReportProgress(99);
            // regist File association
            if (!string.IsNullOrEmpty(Settings.fileAss))
                System.Diagnostics.Process.Start(Path.Combine(dir, Settings.appExeName), Settings.fileAss);
            backgroundWorker2.ReportProgress(100);
        }

        private void backgroundWorker2_ProgressChanged(object sender, ProgressChangedEventArgs e)
        {
            ((Installing)this.stepPanel.Controls[0]).progressBar1.Value = e.ProgressPercentage;
        }

        private void backgroundWorker2_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            ((Installing)this.stepPanel.Controls[0]).progressBar1.Value = 100;
            Installing isi = ((Installing)this.stepPanel.Controls[0]);
            Finish fi = new Finish();
            fi.finishBtn.Click += new EventHandler(finishBtn_Click);
            this.stepPanel.Controls.RemoveAt(0);
            this.stepPanel.Controls.Add(fi);
            isi.Dispose();
            this.inStatus = InstallStatue.ZJInstalled;
            Directory.Delete(System.IO.Path.GetTempPath() + "ZLPInst\\", true);
        }

        private void finishBtn_Click(object sender, EventArgs e)
        {
            if (((Finish)this.stepPanel.Controls[0]).startApp.Checked)
            {
                
                //System.Diagnostics.Process.Start(Path.Combine(dir, Settings.appExeName));
                //System.Diagnostics.ProcessStartInfo Info = new System.Diagnostics.ProcessStartInfo();

                ////设置外部程序名 
                //Info.FileName = Path.Combine(dir, Settings.appExeName);

                ////设置外部程序的启动参数（命令行参数）为test.txt 
                //Info.Arguments = "  ";

                ////设置外部程序工作目录为     C:\ 
                //Info.WorkingDirectory = dir;
                //System.Diagnostics.Process.Start(Info);

                //string argm = "cmd.exe";

               
            }
            this.Close();
        }

        private void CreateShortcuts()
        {
            string path = Path.Combine(Path.Combine(Settings.userStartMenu, Settings.company), Settings.appName);
            if (Directory.Exists(path))
                Directory.Delete(path, true);
            WshShell shell = new WshShell();
            if (Settings.createDesktopshortcut)
            {
                IWshShortcut desktopshortcut = (IWshShortcut)shell.CreateShortcut(Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Desktop), Settings.appName + ".lnk"));

                //设置快捷方式的目标所在的位置(源程序完整路径)
                desktopshortcut.TargetPath = Path.Combine(dir, Settings.appExeName);

                //应用程序的工作目录
                //当用户没有指定一个具体的目录时，快捷方式的目标应用程序将使用该属性所指定的目录来装载或保存文件。
                desktopshortcut.WorkingDirectory = dir;// System.Environment.CurrentDirectory;

                //目标应用程序窗口类型(1.Normal window普通窗口,3.Maximized最大化窗口,7.Minimized最小化)
                desktopshortcut.WindowStyle = 1;

                //快捷方式的描述
                desktopshortcut.Description = Settings.appName;

                //可以自定义快捷方式图标.(如果不设置,则将默认源文件图标.)
                //shortcut.IconLocation = System.Environment.SystemDirectory + "\\" + "shell32.dll, 165";

                //设置应用程序的启动参数(如果应用程序支持的话)
                //shortcut.Arguments = "/myword /d4s";

                //设置快捷键(如果有必要的话.)
                //shortcut.Hotkey = "CTRL+ALT+D";

                //保存快捷方式
                desktopshortcut.Save();
            }
            else
            {
                string deskShortcut = Path.Combine(Environment.GetFolderPath(Environment.SpecialFolder.Desktop), Settings.appName + ".lnk");
                if (System.IO.File.Exists(deskShortcut))
                    System.IO.File.Delete(deskShortcut);
            }
            if (Settings.createStartMenu)
            {
                //CreateDirectory(path, 1);
                Directory.CreateDirectory(path);
                IWshShortcut startMenushortcut = (IWshShortcut)shell.CreateShortcut(Path.Combine(path,Settings.appName+".lnk")/*Environment.GetFolderPath(Environment.SpecialFolder.Desktop) + "//ShortCut.lnk"*/);

                //设置快捷方式的目标所在的位置(源程序完整路径)
                startMenushortcut.TargetPath = Path.Combine(dir, Settings.appExeName);

                //应用程序的工作目录
                //当用户没有指定一个具体的目录时，快捷方式的目标应用程序将使用该属性所指定的目录来装载或保存文件。
                startMenushortcut.WorkingDirectory = dir;// System.Environment.CurrentDirectory;

                //目标应用程序窗口类型(1.Normal window普通窗口,3.Maximized最大化窗口,7.Minimized最小化)
                startMenushortcut.WindowStyle = 1;

                //快捷方式的描述
                startMenushortcut.Description = Settings.appName;

                //可以自定义快捷方式图标.(如果不设置,则将默认源文件图标.)
                //shortcut.IconLocation = System.Environment.SystemDirectory + "\\" + "shell32.dll, 165";

                //设置应用程序的启动参数(如果应用程序支持的话)
                //shortcut.Arguments = "/myword /d4s";

                //设置快捷键(如果有必要的话.)
                //shortcut.Hotkey = "CTRL+ALT+D";

                //保存快捷方式
                startMenushortcut.Save();

                IWshShortcut startMenuuninst = (IWshShortcut)shell.CreateShortcut(Path.Combine(path , "UnInstall.lnk"));

                //设置快捷方式的目标所在的位置(源程序完整路径)
                startMenuuninst.TargetPath = Path.Combine(dir, Settings.unInstallExeName);

                //应用程序的工作目录
                //当用户没有指定一个具体的目录时，快捷方式的目标应用程序将使用该属性所指定的目录来装载或保存文件。
                startMenuuninst.WorkingDirectory = dir;// System.Environment.CurrentDirectory;

                //目标应用程序窗口类型(1.Normal window普通窗口,3.Maximized最大化窗口,7.Minimized最小化)
                startMenuuninst.WindowStyle = 1;

                //快捷方式的描述
                startMenuuninst.Description = Settings.unInstallExeName.Substring(0,Settings.unInstallExeName.Length - 4);

                //可以自定义快捷方式图标.(如果不设置,则将默认源文件图标.)
                //shortcut.IconLocation = System.Environment.SystemDirectory + "\\" + "shell32.dll, 165";

                //设置应用程序的启动参数(如果应用程序支持的话)
                //shortcut.Arguments = "/myword /d4s";

                //设置快捷键(如果有必要的话.)
                //shortcut.Hotkey = "CTRL+ALT+D";

                //保存快捷方式
                startMenuuninst.Save();
            }
            if (Settings.createTaskbar)
            {
                
                //string quickPath = Path.Combine(System.Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData), @"Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar\");
                //IWshShortcut taskbarshortcut = (IWshShortcut)shell.CreateShortcut(Path.Combine(quickPath, Settings.appName + ".lnk")/*Environment.GetFolderPath(Environment.SpecialFolder.Desktop) + "//ShortCut.lnk"*/);

                ////设置快捷方式的目标所在的位置(源程序完整路径)
                //taskbarshortcut.TargetPath = Path.Combine(dir, Settings.appExeName);

                ////应用程序的工作目录
                ////当用户没有指定一个具体的目录时，快捷方式的目标应用程序将使用该属性所指定的目录来装载或保存文件。
                //taskbarshortcut.WorkingDirectory = dir;// System.Environment.CurrentDirectory;

                ////目标应用程序窗口类型(1.Normal window普通窗口,3.Maximized最大化窗口,7.Minimized最小化)
                //taskbarshortcut.WindowStyle = 1;

                ////快捷方式的描述
                //taskbarshortcut.Description = Settings.appName;

                ////可以自定义快捷方式图标.(如果不设置,则将默认源文件图标.)
                ////shortcut.IconLocation = System.Environment.SystemDirectory + "\\" + "shell32.dll, 165";

                ////设置应用程序的启动参数(如果应用程序支持的话)
                ////shortcut.Arguments = "/myword /d4s";

                ////设置快捷键(如果有必要的话.)
                ////shortcut.Hotkey = "CTRL+ALT+D";

                ////保存快捷方式
                //taskbarshortcut.Save();

               //ShellExecute(NULL, "taskbarpin", "c:\users\username\Desktop\IE.lnk", NULL, NULL, 0)
            }
        }
        private void CreateDirectory(string dir, int checkLevel)
        {
            while (checkLevel > 0)
            {
                string tmpDir = dir;
                for (int i = 0; i < checkLevel; i++)
                {
                    tmpDir = tmpDir.Substring(0, tmpDir.LastIndexOf("\\"));
                }
                if (!Directory.Exists(tmpDir))
                    Directory.CreateDirectory(tmpDir);
                checkLevel--;
            }
            if (checkLevel <= 0)
                if (!Directory.Exists(dir))
                    Directory.CreateDirectory(dir);
        }
        private void CreateUserShortcuts(string path,string name,string targetPath, string WorkingDirectory, int windowStyle,
            string description, string icoLocation)
        {
            WshShell shell = new WshShell();
            if (Settings.createDesktopshortcut)
            {
                IWshShortcut desktopshortcut = (IWshShortcut)shell.CreateShortcut(Path.Combine(path, name+".lnk"));

                //设置快捷方式的目标所在的位置(源程序完整路径)
                desktopshortcut.TargetPath = targetPath;

                //应用程序的工作目录
                //当用户没有指定一个具体的目录时，快捷方式的目标应用程序将使用该属性所指定的目录来装载或保存文件。
                desktopshortcut.WorkingDirectory = WorkingDirectory;// System.Environment.CurrentDirectory;

                //目标应用程序窗口类型(1.Normal window普通窗口,3.Maximized最大化窗口,7.Minimized最小化)
                desktopshortcut.WindowStyle = windowStyle;

                //快捷方式的描述
                desktopshortcut.Description = description;

                //可以自定义快捷方式图标.(如果不设置,则将默认源文件图标.)
                desktopshortcut.IconLocation = icoLocation;// System.Environment.SystemDirectory + "\\" + "shell32.dll, 165";

                //设置应用程序的启动参数(如果应用程序支持的话)
                //shortcut.Arguments = "/myword /d4s";

                //设置快捷键(如果有必要的话.)
                //shortcut.Hotkey = "CTRL+ALT+D";

                //保存快捷方式
                desktopshortcut.Save();
            }
        }
    }

}
