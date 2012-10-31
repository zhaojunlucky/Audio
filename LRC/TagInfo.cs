using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace LRC
{
    public class TagInfo
    {
        private string name;
        private string path;
        public TagInfo(string name,string path)
        {
            this.path = path;
            this.name = name;
        }

        public string Path
        {
            get
            {
                return this.path;
            }
            set
            {
                this.path = value;
            }
        }

        public string Name
        {
            get
            {
                return this.name;
            }
            set
            {
                this.name = value;
            }
        }
    }
}
