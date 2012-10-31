using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Forms;

namespace LRC
{
    public class SaveLrc
    {
        public static void SaveLrcFile(string path, TextEncodings encoding, string[] lrc)
        {
            if (File.Exists(path))
            {
                File.Delete(path);
            }

            StreamWriter writer = new StreamWriter(path, false, GetEncoding(encoding));
            foreach (string o in lrc)
            {
                writer.WriteLine(o);
            }
            writer.Flush();
            writer.Close();
        }

        public static void SaveLrcFile(string path, TextEncodings encoding, string lrc)
        {          
            if (File.Exists(path))
            {
                File.Delete(path);
            }

            try
            {
                StreamWriter writer = new StreamWriter(path, false, GetEncoding(encoding));
                writer.WriteLine(lrc);
                writer.Flush();
                writer.Close();
            }
            catch (System.Exception ex)
            {
                MessageBox.Show(ex.Message);
            }
            
        }

        public static Encoding GetEncoding(TextEncodings encording)
        {
            switch (encording)
            {
                case TextEncodings.GBK:
                    return Encoding.GetEncoding("GBK");
                case TextEncodings.UTF_16:
                    return Encoding.GetEncoding("UTF-16");
                case TextEncodings.UTF_16BE:
                    return Encoding.GetEncoding("UTF-16BE");
                case TextEncodings.UTF_8:
                    return Encoding.UTF8;
                default:
                    return Encoding.Default;
            }
        }
    }
}
