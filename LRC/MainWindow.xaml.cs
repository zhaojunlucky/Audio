using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.IO;
using System.Collections.Specialized;
using System.Threading;
using System.Data;
using System.Xml;
using System.ComponentModel;
using Microsoft.Win32;
using System.Windows.Interop;
using LRC;
using LRC.Shell;
using System.Runtime.InteropServices;
using System.IO.Pipes;

namespace LRC
{

    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        #region collections
        private List<MusicFile> listViewFileList;
        private List<MusicFile> fileList;
        private List<MusicFile> failedList;
        private StringCollection dirList = new StringCollection();
        #endregion

        #region downloads
        private LRCDownload lrcDownload;
        private Thread thSearch;
        private Thread thDownload;
        private LRCBase selectLRCBase;
        private Semaphore signal;
        #endregion

        #region delegates
        private delegate void UpdateStatus(string mes);
        private delegate void AddLrcSelect(List<LRCBase> lrcbaselist, MusicFile music);
        private delegate void AddLrc(string lrc);
        private delegate void AddFileToListView(string name, MusicFile musicFile);
        internal delegate void SearchLrcByZPL(string title, string artist, string file);

        private UpdateStatus updateStatus;
        private AddLrcSelect addLrcSelect;
        private AddLrc addLrc;     
        private BackgroundWorker backgroundWorker;
        private AddFileToListView addFileToListView;
        internal SearchLrcByZPL searchLrcByZLP;
        private delegate void ScanComplete();
        private ScanComplete scanComplete;
        static System.Threading.Mutex mtx;
        private Thread psSer = null;
        private PipeServer ps;
        #endregion

        #region others
        private DataTable table;
        private WaitingWindow waitingWindows;
        private string[] extension = new string[] { ".mp3" ,".ape",".flac",".aac"};
        private string defaultPath;
        private TextEncodings defaultEncoding = TextEncodings.ASCII;
        private ContextMenu contextMenu;
        private ContextMenu dirViewContextMenu;
        private ContextMenu fileListViewContextMenu;
        private string defaultName = "";
        private bool searchFromZLP = false;
        #endregion

        public MainWindow()
        {
            InitializeComponent();


            bool isFirst;
            mtx  = new System.Threading.Mutex
                                                               (true, "zjdd ", out   isFirst);

            #region Background color
            this.Background = new SolidColorBrush(Color.FromRgb(188, 99, 163));
            this.mainPanel.Background = this.Background;
            this.fileListView.Background = new SolidColorBrush(Color.FromRgb(213, 133, 191));
            this.about.Background = new SolidColorBrush(Color.FromRgb(213, 133, 191));
            this.lrcBox.Background = new SolidColorBrush(Color.FromRgb(213, 133, 191));
            this.lrcList.Background = new SolidColorBrush(Color.FromRgb(213, 133, 191));
            this.directoryTreeView.TreeViewBackground = new SolidColorBrush(Color.FromRgb(213, 133, 191));
            #endregion

            #region Initialize directory treeview contexmenu
            dirViewContextMenu = new ContextMenu();
            MenuItem menu = new MenuItem();
            menu.Header = "Add This Folder";
            menu.Click += new RoutedEventHandler(menu_Click);
            dirViewContextMenu.Items.Add(menu);
            MenuItem open = new MenuItem();
            open.Header = "Open Folder in Windows Explorer";
            open.Click += new RoutedEventHandler(open_Click);
            dirViewContextMenu.Items.Add(open);
            MenuItem properties = new MenuItem();
            properties.Header = "Properties";
            properties.Click += new RoutedEventHandler(properties_Click);
            dirViewContextMenu.Items.Add(properties);
            this.directoryTreeView.TreeViewContextMenu = dirViewContextMenu;
            #endregion

            #region Initialize mainwindow icon
            defaultPath = Settings.Settings.Default.lrcDirectory;//
            if ("" == defaultPath.Trim())
            {
                defaultPath = System.AppDomain.CurrentDomain.SetupInformation.ApplicationBase + "ZDLRC\\";
                Settings.Settings.Default.lrcDirectory = defaultPath;
            }
            MemoryStream stream = new MemoryStream();
            Properties.Resources.lrc.Save(stream, System.Drawing.Imaging.ImageFormat.Png);
            ImageSourceConverter imageSourceConverter = new ImageSourceConverter();
            this.Icon = (ImageSource)imageSourceConverter.ConvertFrom(stream);
            


          

            Image image = new Image();
            image.Source = ImageHelper.ToBitmapSource(Properties.Resources.lrc);
            image.Height = image.Width = 64;


            #endregion

            #region delegate and thread           
            fileList = new List<MusicFile>();
            lrcDownload = new LRCDownload();
            updateStatus = new UpdateStatus(UpdateSearchStatus);
            addLrcSelect = new AddLrcSelect(AddSearchLrc);
            addLrc = new AddLrc(AddDownloadLRC);
            failedList = new List<MusicFile>();
            searchLrcByZLP = new SearchLrcByZPL(SearchFromZLP);
            listViewFileList = new List<MusicFile>();
            fileList = new List<MusicFile>();
            lrcDownload = new LRCDownload();
            selectLRCBase = new LRCBase();
            scanComplete = new ScanComplete(ScanFinish);
            addFileToListView = new AddFileToListView(UpdateScanResult);
            backgroundWorker = new BackgroundWorker();
            backgroundWorker.DoWork += new DoWorkEventHandler(backgroundWorker_DoWork);
            backgroundWorker.RunWorkerCompleted += new RunWorkerCompletedEventHandler(backgroundWorker_RunWorkerCompleted);
            #endregion

            #region Initialize aboutBox and load settings
            autoSelect.IsChecked = Settings.Settings.Default.autoSave;

            about.AppendText("LRC Download 1.0.0.1\n\n");
            about.AppendText("This is a free soft made by ZJ.\n");
            about.AppendText("If you have questions,please send me email:zhaojunlucky@qq.com.\n");
            about.AppendText("Copyright© 2010-2012 of ZJSoft\n");
            new InlineUIContainer(image, about.Document.ContentEnd);
            #endregion

            #region Initialize lrcBox contexmenu
            InitLrcBoxContextMenu();
            #endregion

            #region Initialize fileListViewContextmenu
            fileListViewContextMenu = new ContextMenu();
            MenuItem clear = new MenuItem();
            clear.Header="Remove All Files";
            clear.Click += new RoutedEventHandler(clear_Click);
            fileListViewContextMenu.Items.Add(clear);
            fileListViewContextMenu.Opened += new RoutedEventHandler(fileListViewContextMenu_Opened);
            fileListView.ContextMenu = fileListViewContextMenu;
            #endregion    

            ps = new PipeServer(this);
            psSer = new Thread(new ThreadStart(ps.StartPipeServer));
            psSer.Start();
            //MessageBox.Show(App.args);
            if (!String.IsNullOrEmpty(App.args))
            {
                string[] pas = App.args.Split('|');
                if (pas.Length >= 3)
                {
                    if (pas[0].Trim() == "0xFFFF")
                        pas[0] = "";
                    if (pas[1].Trim() == "0xFFFF")
                        pas[1] = "";
                   
                    SearchFromZLP(pas[0], pas[1], pas[2]);
                }
            }
        }





        #region fileListViewContextmenu event
        void fileListViewContextMenu_Opened(object sender, RoutedEventArgs e)
        {
            DataTable t = fileListView.DataContext as DataTable;
            if (null == t || t.Rows.Count <= 0)
            {
                ((MenuItem)fileListViewContextMenu.Items[0]).IsEnabled = false;
            }
            else
            {
                ((MenuItem)fileListViewContextMenu.Items[0]).IsEnabled = searchByDir.IsEnabled;
            }
        }

        private void clear_Click(object sender, RoutedEventArgs e)
        {
            table = fileListView.DataContext as DataTable;
            table.Rows.Clear();
            fileListView.DataContext = table;
            table = null;
            dirList.Clear();
            listViewFileList.Clear();
            fileInfo.Content = "Copyright© 2010 of ZJSoft";
        }
        #endregion
     
        #region Directory TreeView ContexMenu event
        public void properties_Click(object sender, RoutedEventArgs e)
        {
            SystemProperties.ShowProperties(IntPtr.Zero, this.directoryTreeView.SelectPath);
        }

        public void open_Click(object sender, RoutedEventArgs e)
        {
            if (null == this.directoryTreeView.SelectedItem)
            {
                return;
            }

            System.Diagnostics.Process.Start(((TagInfo)(((TreeViewItem)this.directoryTreeView.SelectedItem).Tag)).Path);
        }

        public void menu_Click(object sender, RoutedEventArgs e)
        {
            if (null != this.directoryTreeView.SelectedItem)
            {
                AddSeletctFolderHander(((TagInfo)(((TreeViewItem)this.directoryTreeView.SelectedItem).Tag)).Path);
            }
        }
        #endregion

        #region LrcBox contexmenu
        private void InitLrcBoxContextMenu()
        {
            contextMenu = new ContextMenu();
            MenuItem copy = new MenuItem();
            copy.Header = "Copy";
            copy.Click += new RoutedEventHandler(copy_Click);
            contextMenu.Items.Add(copy);
            MenuItem save = new MenuItem();
            save.Header = "Save";
            save.Click += new RoutedEventHandler(save_Click);
            contextMenu.Items.Add(save);
            contextMenu.Opened += new RoutedEventHandler(contextMenu_Opened);
            lrcBox.ContextMenu = contextMenu;
        }

        private void contextMenu_Opened(object sender, RoutedEventArgs e)
        {
            ((MenuItem)contextMenu.Items[0]).IsEnabled = (lrcBox.Selection.Text != "");
            TextRange textRange = new TextRange(lrcBox.Document.ContentStart, lrcBox.Document.ContentEnd);
            ((MenuItem)contextMenu.Items[1]).IsEnabled = (textRange.Text.Trim() != "");
        }

        private void save_Click(object sender, RoutedEventArgs e)
        {
            SaveFileDialog saveDialog = new SaveFileDialog();
            saveDialog.Filter = "LRC File|*.LRC";
            saveDialog.Title = "Save LRC file";
            saveDialog.FileName = this.titleBox.Text;
            if ((bool)saveDialog.ShowDialog())
            {
                string fileName = saveDialog.FileName;
                TextRange textRange = new TextRange(lrcBox.Document.ContentStart, lrcBox.Document.ContentEnd);


                SaveLrc.SaveLrcFile(fileName, TextEncodings.UTF_8, textRange.Text);
            }
        }

        private void copy_Click(object sender, RoutedEventArgs e)
        {
            lrcBox.Copy();
        }
        #endregion

        #region BackgroundWork
        private void ScanFinish()
        {
            fileListView.DataContext = table;
            searchByDir.IsEnabled = (table.Rows.Count > 1);
            if (null != waitingWindows)
            {
                waitingWindows.canCLose = true;
                waitingWindows.Close();
                waitingWindows = null;
            }
        }

        private void backgroundWorker_RunWorkerCompleted(object sender, RunWorkerCompletedEventArgs e)
        {
            this.Dispatcher.Invoke(scanComplete);
        }

        private void UpdateScanResult(string filename, MusicFile musicFile)
        {
            if (!listViewFileList.Contains(musicFile))
            {
                listViewFileList.Add(musicFile);

                table.Rows.Add(filename, musicFile.Title, musicFile.Artist, musicFile.Path);
            }
        }

        private void backgroundWorker_DoWork(object sender, DoWorkEventArgs e)
        {
            string path = e.Argument as string;
            string[] files = Directory.GetFiles(path);
            foreach (string o in files)
            {
                FileInfo info = new FileInfo(o);
                string title = "";
                string artist = "";
                if (extension.Contains(info.Extension))
                {
                    ID3v2Tag tag = new ID3v2Tag(o);
                    artist = tag.artist;
                    title = tag.title;
                    MusicFile musicFile = new MusicFile(o, title, artist);
                    this.Dispatcher.Invoke(addFileToListView, info.Name, musicFile);
                }
            }
            
        }
        #endregion

        #region delegate invoke function
        public void UpdateSearchStatus(string mes)
        {
            status.Content = mes;
        }

        public void AddSearchLrc(List<LRCBase> lrcbaselist,MusicFile music)
        {
            //
            lrcList.DataContext = CreateDataTable(lrcbaselist);
            status.Content = string.Format("{0} lrc searched", lrcbaselist.Count);
            if (expander1.IsExpanded && (bool)autoSelect.IsChecked && lrcbaselist.Count >0)
            {
                selectLRCBase = null;
                selectLRCBase = lrcbaselist[0];
                foreach (LRCBase lrcBase in lrcbaselist)
                {
                    if (lrcBase.Singer == music.Artist && lrcBase.SongName == music.Title)
                    {
                        selectLRCBase = lrcBase;
                        break;
                    }
                }
                if (null != thDownload)
                {
                    thDownload.Abort();
                }
                thDownload = new Thread(new ThreadStart(DownloadThread));
                thDownload.Start();
            }
            if (music == fileList[fileList.Count - 1] && expander1.IsExpanded)
            {
                searchByDir.IsEnabled = true;
            }
            //
        }

        public void AddDownloadLRC(string lrc)
        {
            //
            lrcBox.SelectAll();
            lrcBox.Selection.Text = lrc;
            if (thSearch != null && thSearch.ThreadState == ThreadState.WaitSleepJoin && null != signal)
            {
                signal.Release();
            }
            status.Content = Messages.Downloaded;
            //
        }

        public void SearchFromZLP(string title, string artist, string file)
        {
            searchFromZLP = true;
            // do search
            title = title.Trim();
            artist = artist.Trim();
            
            this.titleBox.Text = (title == "0xFFFF"?"":title);
            this.aritstBox.Text = (artist == "0xFFFF"?"":artist);
            defaultName = file; 
            searchByEnter_Click(this, new RoutedEventArgs());
        }
        #endregion

        private DataTable CreateDataTable(List<LRCBase> lrcbaselist)
        {
            DataTable tbl = new DataTable("LrcList");

            tbl.Columns.Add("ID", typeof(int));
            tbl.Columns.Add("Title", typeof(string));
            tbl.Columns.Add("Artist", typeof(string));
            
            foreach (LRCBase lrcBase in lrcbaselist)
            {
                tbl.Rows.Add(lrcBase.ID, lrcBase.SongName, lrcBase.Singer);
            }

            return tbl;
        }

        private void AddSeletctFolderHander( string folderPath)
        {
            if (Directory.Exists(folderPath) && !dirList.Contains(folderPath))
            {
                if (!expander1.IsExpanded)
                {
                    expander1.IsExpanded = true;
                }
                dirList.Add(folderPath);             
                table = null;
                if (fileListView.DataContext == null)
                {
                    table = new DataTable("FileList");
                    table.Columns.Add("Name", typeof(string));
                    table.Columns.Add("Title", typeof(string));
                    table.Columns.Add("Artist", typeof(string));
                    table.Columns.Add("Path", typeof(string));
                }
                else
                {
                    table = fileListView.DataContext as DataTable;
                }
                backgroundWorker.RunWorkerAsync(folderPath);
                if (waitingWindows == null)
                {
                    waitingWindows = new WaitingWindow();
                }
                waitingWindows.Owner = this;
                waitingWindows.ShowDialog();
            }
        }

        #region expander event
        private void expander1_Expanded(object sender, RoutedEventArgs e)
        {
            expander2.IsExpanded = !expander1.IsExpanded;
        }

        private void expander1_Collapsed(object sender, RoutedEventArgs e)
        {
            expander2.IsExpanded  = !expander1.IsExpanded;
            about.Visibility = System.Windows.Visibility.Visible;
        }

        private void expander2_Collapsed(object sender, RoutedEventArgs e)
        {
            about.Visibility = System.Windows.Visibility.Collapsed;
            expander1.IsExpanded = !expander2.IsExpanded;
            fileInfo.Visibility = System.Windows.Visibility.Visible;
        }

        private void expander2_Expanded(object sender, RoutedEventArgs e)
        {
            fileInfo.Visibility = System.Windows.Visibility.Collapsed;
            fileInfo.Content = "Copyright© 2010 of ZJSoft";
            expander1.IsExpanded = !expander2.IsExpanded;
        }
        #endregion

        #region Search Download
        private void DoSearch()
        {
            if (null != thSearch)
            {
                thSearch.Abort();
            }
            if (null == this.signal)
            {
                this.signal = new Semaphore(1, 1);
            }
            else
            {
                ///this.Close();
                this.signal.Close();
                this.signal = new Semaphore(1, 1);
            }
            thSearch = new Thread(new ThreadStart(SearchThread));
            thSearch.Start();
        }

        public void SearchThread()
        {
            failedList.Clear();
            foreach (MusicFile music in fileList)
            {
                if (music.IsChecked)
                {
                    signal.WaitOne();
                    this.Dispatcher.Invoke(updateStatus, Messages.Searching);
                    List<LRCBase> lrcbaselist = lrcDownload.DownloadLrcList(music.Title, music.Artist);
                    this.Dispatcher.Invoke(addLrcSelect, lrcbaselist, music);
                }
            }
        }

        public static string SBCToDBC(string input)
        {
            char[] cc = input.ToCharArray();
            for (int i = 0; i < cc.Length; i++)
            {
                if (cc[i] == 12288)
                {
                    cc[i] = (char)32;
                    continue;
                }
                if (cc[i] > 65280 && cc[i] < 65375)
                {
                    cc[i] = (char)(cc[i] - 65248);
                }

            }
            return new string(cc);
        }  

        public void DownloadThread()
        {
            if (null == selectLRCBase)
            {
                return;
            }
            this.Dispatcher.Invoke(updateStatus,Messages.Downloading);
            string lrc = lrcDownload.DownloadLrc(selectLRCBase);
            if (!Directory.Exists(defaultPath))
            {
                Directory.CreateDirectory(defaultPath);
            }
            //SaveLrc.SaveLrcFile(System.IO.Path.Combine(defaultPath, SBCToDBC(selectLRCBase.SongName) + ".lrc"), defaultEncoding, lrc);
            this.Dispatcher.Invoke(addLrc, lrc);
        }

        #endregion

        #region UI search and download click event
        private void lrcList_MouseDoubleClick(object sender, MouseButtonEventArgs e)
        {
            if (this.lrcList.SelectedIndex < 0)
                return;
            DataTable table = this.lrcList.DataContext as DataTable;
            DataRow row = table.Rows[this.lrcList.SelectedIndex];
            object []info = row.ItemArray;
            selectLRCBase = null;
            selectLRCBase = new LRCBase(Convert.ToInt32(info[0]), info[1].ToString(), info[2].ToString());
            if (null != thDownload)
            {
                thDownload.Abort();
            }
            thDownload = new Thread(new ThreadStart(DownloadThread));
            thDownload.Start();
        }

        
        private void searchByEnter_Click(object sender, RoutedEventArgs e)
        {            
            if (titleBox.Text.Trim() == "")
            {
                return;
            }
            fileList.Clear();
            fileList.Add(new MusicFile("", titleBox.Text, aritstBox.Text));
            DoSearch();
        }

        private void searchByDir_Click(object sender, RoutedEventArgs e)
        {
            fileList = listViewFileList;
            if (listViewFileList.Count <= 0)
            {
                return;
            }
            searchByDir.IsEnabled = false;
            DoSearch();
        }

        private void titleBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                aritstBox.Focus();
            }
        }

        private void aritstBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key == Key.Enter)
            {
                searchByEnter_Click(searchByEnter, new RoutedEventArgs());
            }
        }
        #endregion

        #region fileListView selection event

        private void fileListView_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            DataRowView row = (DataRowView)fileListView.SelectedItem;
            if (fileInfo.Visibility == System.Windows.Visibility.Visible && null!=row)
            {
                object[]info = row.Row.ItemArray;
                fileInfo.Content = string.Format("FileName:{0}, Title:{0}, Artist:{0}", info[0], info[1], info[2]);
                fileInfo.ToolTip = fileInfo.Content;
            }
        }

        private void CheckBox_Checked(object sender, RoutedEventArgs e)
        {
            string path = (sender as CheckBox).Tag.ToString();
            foreach (MusicFile musicFile in listViewFileList)
            {
                if (musicFile.Path == path)
                {
                    musicFile.IsChecked = true;
                    break;
                }
            }
        }

        private void CheckBox_Unchecked(object sender, RoutedEventArgs e)
        {
            string path = (sender as CheckBox).Tag.ToString();
            foreach (MusicFile musicFile in listViewFileList)
            {
                if (musicFile.Path == path)
                {
                    musicFile.IsChecked = false;
                    break;
                }
            }
        }
        #endregion

        #region Window Setting
        private void Window_Closing(object sender, System.ComponentModel.CancelEventArgs e)
        {
            ps.running = false;
            PipeServer.SendPipeMessgae("exit|exit|exit");
             
            Settings.Settings.Default.autoSave = (bool)autoSelect.IsChecked ;
            if (null != thDownload)
            {
                thDownload.Abort();
            }
            if (null != thSearch)
            {
                thSearch.Abort();
            }
            mtx.Close();
        }

       

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            #region add a menu to sysmenu
            //int i = new WindowInteropHelper(this).Handle.ToInt32();
            IntPtr handle = ((HwndSource)PresentationSource.FromVisual(this)).Handle;
            IntPtr sysMenuHandle = API.GetSystemMenu(handle, 0);
            API.AppendMenu(sysMenuHandle, 0, 0, null);
            API.AppendMenu(sysMenuHandle, 0, 1987, "Settings");
            #endregion
            HwndSource source = HwndSource.FromHwnd(new WindowInteropHelper(this).Handle);
            source.AddHook(new HwndSourceHook(WndProc));
        }

        private IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            // Handle whatever Win32 message it is we feel like handling
            if (msg == 0x112)
            {
                if (wParam.ToInt32() == 1987)
                {
                    SettingWindow settingWindow = new SettingWindow();
                    settingWindow.Owner = this;
                    settingWindow.ShowDialog();
                    this.defaultPath = Settings.Settings.Default.lrcDirectory;
                }
            }
            else if (msg == 0x0219)
            {

            }

            return IntPtr.Zero;
        }
        #endregion



   
    }
}
