using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;

namespace LRC
{
    public class MusicFile
    {
        private string path;
        private string title;
        private string artist;
        private bool isChecked = true;
        public MusicFile(string path, string title, string artist)
        {
            //if (!File.Exists(path))
            //{
            //    throw new Exception(string.Format("File {0} Not Exist!", path));
            //}
            this.path = path;
            this.title = title;
            this.artist = artist;
        }

        public MusicFile(string path, string title, string artist,bool isChecked)
        {
            //if (!File.Exists(path))
            //{
            //    throw new Exception(string.Format("File {0} Not Exist!", path));
            //}
            this.path = path;
            this.title = title;
            this.artist = artist;
            this.isChecked = isChecked;
        }

        public bool IsChecked
        {
            get
            {
                return this.isChecked;
            }
            set
            {
                this.isChecked = value;
            }
        }
        public string Path
        {
            get
            {
                return path;
            }
        }
        public string Title
        {
            get
            {
                return title;
            }
            set
            {
                title = value;
            }
        }

        public string Artist
        {
            get
            {
                return artist;
            }
            set
            {
                artist = value;
            }
        }
    }
}
