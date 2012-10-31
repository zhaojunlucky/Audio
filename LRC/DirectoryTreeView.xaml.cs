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
using System.Windows.Interop;
using System.IO;
using Microsoft.WindowsAPICodePack.Shell;

namespace LRC
{
    /// <summary>
    /// Interaction logic for DirectoryTreeView.xaml
    /// </summary>
    public partial class DirectoryTreeView : UserControl
    {
        ShellImageList list = new ShellImageList();
        private string selectPath;
        public delegate void AddSeletctFolder(object sender, string folderPath);
        public event AddSeletctFolder AddSeletctFolderHander;
        private ContextMenu dirTreeViewContextMenu = null;
            
        public DirectoryTreeView( )
        {
            InitializeComponent();
           
            dirView.SelectedItemChanged += new RoutedPropertyChangedEventHandler<object>(dirView_SelectedItemChanged);   
            InitDirTreeView();
        }

        public Brush TreeViewBackground
        {
            get
            {
                return this.dirView.Background;
            }
            set
            {
                this.dirView.Background = value;
            }
        }
        
        public ContextMenu TreeViewContextMenu
        {
            get
            {
                return this.dirTreeViewContextMenu;
            }
            set
            {
                this.dirTreeViewContextMenu = value;
                // 
                for (int i = 0; i < dirView.Items.Count; i++)
                {
                    TreeViewItem itemParent = (TreeViewItem)dirView.Items[i];
                    for (int j = 0; j < itemParent.Items.Count; j++)
                    {
                        ((TreeViewItem)itemParent.Items[j]).ContextMenu = this.dirTreeViewContextMenu;
                    }
                }
            }
        }

        public void dirView_SelectedItemChanged(object sender, RoutedPropertyChangedEventArgs<object> e)
        {
            TagInfo info = (TagInfo)(((TreeViewItem)dirView.SelectedItem).Tag);
            this.selectPath = info.Path;
        }

        public string SelectPath
        {
            get
            {
                return this.selectPath;
            }
        }

        public TreeViewItem SelectedItem
        {
            get
            {
                return (TreeViewItem)dirView.SelectedItem;
            }
        }

        private void UserControl_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            dirView.Width = this.Width;
            dirView.Height = this.Height;
        }

        private void InitDirTreeView()
        {
            AddLibraries();
            
            // add computer
            AddComputer();
        }

        private TreeViewItem AddLibraries()
        {
            StackPanel panel = new StackPanel();
            IKnownFolder iKnownFolder = KnownFolders.UsersLibraries;
            ShellObject shellObject = ShellFolder.FromParsingName(iKnownFolder.ParsingName);
            panel.Orientation = Orientation.Horizontal;
            Image image = new Image();
            image.Source = shellObject.Thumbnail.SmallBitmapSource;
            image.Width = image.Height = 16;
            panel.Children.Add(image);
            TagInfo info = new TagInfo(shellObject.Name, iKnownFolder.Path);
            TextBlock textBlock = new TextBlock();
            textBlock.Text = info.Name;
            panel.Children.Add(textBlock);
            TreeViewItem item = new TreeViewItem();
            item.Header = panel;
            item.Tag = info;
            dirView.Items.Add(item);
            item.IsExpanded = true;
            //item.Items.Add(CreatTreeViewItem(new TagInfo("Documents", Environment.GetFolderPath(Environment.SpecialFolder.Personal))));
            //item.Items.Add(CreatTreeViewItem(new TagInfo("Music", Environment.GetFolderPath(Environment.SpecialFolder.MyMusic))));
            //item.Items.Add(CreatTreeViewItem(new TagInfo("Pictures", Environment.GetFolderPath(Environment.SpecialFolder.MyPictures))));
            //item.Items.Add(CreatTreeViewItem(new TagInfo("Videos", Environment.GetFolderPath(Environment.SpecialFolder.MyVideos))));
            //item.Items.Add(CreatTreeViewItem(new TagInfo("Downloads", "")));
            foreach (var shellObj in iKnownFolder)
            {
                try
                {
                    ShellLibrary shellLibrary = ShellLibrary.Load(shellObj.Name, true);
                    if (shellLibrary.Count > 0 && Directory.Exists(shellLibrary.DefaultSaveFolder))
                        item.Items.Add(CreatTreeViewItem(new TagInfo(shellObj.Name, shellLibrary.DefaultSaveFolder)));
                }
                catch
                {
                }
            }
            return item;
        }

        private TreeViewItem AddComputer( )
        {
            StackPanel panel = new StackPanel();
            panel.Orientation = Orientation.Horizontal;
            IKnownFolder iKnownFolder = KnownFolders.Computer;
            ShellObject shellObject = ShellFolder.FromParsingName(iKnownFolder.ParsingName);
            Image image = new Image();
            image.Source = shellObject.Thumbnail.SmallBitmapSource;
            image.Width = Height = 16;
            panel.Children.Add(image);
            TagInfo info = new TagInfo(shellObject.Name, iKnownFolder.Path);
            
            TextBlock textBlock = new TextBlock();
            textBlock.Text = info.Name;
            panel.Children.Add(textBlock);
            TreeViewItem item = new TreeViewItem();
            item.Header = panel;
            item.Tag = info;
            item.IsSelected = true;
            dirView.Items.Add(item);
            item.IsExpanded = true;

            foreach (var obj in iKnownFolder)
            {
                if(Directory.Exists(obj.ParsingName))
                {
                    item.Items.Add(CreatTreeViewItem(new TagInfo(obj.Name, obj.ParsingName)));
                }
            }
            //System.IO.DriveInfo[] disk = System.IO.DriveInfo.GetDrives();
            //foreach (System.IO.DriveInfo di in disk)
            //{
            //    if (di.DriveType != DriveType.CDRom && di.DriveType != DriveType.Network)
            //    {
            //        string name = "(" + di.Name.Substring(0, 2) + ")";
            //        string lable = (di.VolumeLabel == "") ? "Local Disk" : di.VolumeLabel;
            //        item.Items.Add(CreatTreeViewItem(new TagInfo(lable + name, di.RootDirectory.FullName)));
            //    }
            //}

            
            return item;
        }

        private TreeViewItem CreatTreeViewItem(TagInfo info)
        {
            StackPanel panel = new StackPanel();
            panel.Orientation = Orientation.Horizontal;
            Image image = new Image();
            image.Source = list.GetDirectoryImage(info.Path, true);
            image.Height = image.Width = 16;
            panel.Children.Add(image);
            TextBlock textBlock = new TextBlock();
            textBlock.Text = info.Name;
            panel.Children.Add(textBlock);
            TreeViewItem item = new TreeViewItem();
            item.Header = panel;
            item.Tag = info;
            item.ToolTip = info.Name;
            TreeViewItem tmpitem = new TreeViewItem();
            tmpitem.Header = "@@";           
            item.Expanded += new RoutedEventHandler(item_Expanded);
            if (null != this.dirTreeViewContextMenu)
            {
                item.ContextMenu = this.dirTreeViewContextMenu;//;;'
            }
            item.Items.Add(tmpitem);           
            return item;
        }

        void item_Expanded(object sender, RoutedEventArgs e)
        {
            TreeViewItem item = e.Source as TreeViewItem;
            if (null == item.Tag)
            {
                return;
            }
            if (item.Items.Count == 1)
            {
                if (null == ((TreeViewItem)item.Items[0]).Tag)
                {
                    // load dirs
                    item.Items.Clear();
                    TagInfo info = (TagInfo)item.Tag;
                    if (Directory.Exists(info.Path))
                    {
                        try
                        {
                            string[] dirs = Directory.GetDirectories(info.Path);

                            foreach (string dir in dirs)
                            {
                                DirectoryInfo dirInfo = new DirectoryInfo(dir);
                                FileAttributes attribute = dirInfo.Attributes;
                                if (!attribute.HasFlag(FileAttributes.Hidden))
                                {
                                    item.Items.Add(CreatTreeViewItem(new TagInfo(dirInfo.Name, dir)));
                                }
                            }
                        }
                        catch
                        {
                        }
                    }
                }
            }
        }
    }
}
