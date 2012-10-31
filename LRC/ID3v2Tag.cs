using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;

namespace LRC
{
  


    public class ID3v2Tag
    {
        [DllImport("Audio.dll", EntryPoint = "CreateZLP", ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        private static extern IntPtr CreateZLP();

        [DllImport("Audio.dll", EntryPoint = "CloseZLP", ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        private static extern void CloseZLP(IntPtr ptr);

        [DllImport("Audio.dll", EntryPoint = "OpenFile", ExactSpelling = true, CallingConvention = CallingConvention.StdCall)]
        private static extern int OpenFile(IntPtr ptr,  String file);

        [DllImport("Audio.dll", EntryPoint = "GetTitle",  CallingConvention = CallingConvention.StdCall)]
        private static extern void GetTitle(IntPtr ptr,  StringBuilder title,Int32 size);

        [DllImport("Audio.dll", EntryPoint = "GetArtist",  CallingConvention = CallingConvention.StdCall)]
        private static extern void GetArtist(IntPtr ptr,  StringBuilder artist,Int32 size);

        private IntPtr hwnd;
        public string title;
        public string artist;
        public ID3v2Tag(string file)
        {
            hwnd = CreateZLP();
            if (0 == OpenFile(hwnd, file))
            {
                //GetTitle(hwnd, ref title);[MarshalAs(UnmanagedType.LPTStr)] 
                //GetArtist(hwnd, artist);
                StringBuilder b = new StringBuilder(1024);
               
                GetArtist(hwnd, b,1024);
                artist = b.ToString();
                GetTitle(hwnd, b,1024);
                title = b.ToString();
            }
            if (null == title)
                title = "";
            if (null == artist)
                artist = "";
        
            CloseZLP(hwnd);
        }
    }
}
