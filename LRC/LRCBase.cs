using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LRC
{
    public class LRCBase
    {
        private int id;
        private string song;
        private string singer;

        public LRCBase()
        {
            this.id = 0;
            this.song = "";
            this.singer = "";
        }

        public LRCBase(int _id, string _song, string _singer)
        {
            this.id = _id;
            this.song = _song;
            this.singer = _singer;
        }

        public int ID
        {
            get
            {
                return id;
            }
            set
            {
                id = value;
            }
        }

        public string SongName
        {
            get
            {
                return song;
            }
            set
            {
                song = value;
            }
        }

        public string Singer
        {
            get
            {
                return singer;
            }
            set
            {
                singer = value;
            }
        }
    }
}
