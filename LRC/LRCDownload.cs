using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;
using System.Net;
using System.IO;

namespace LRC
{
    public class LRCDownload
    {
        public static readonly string SearchURL = "http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?sh?Artist={0}&Title={1}&Flags=0";
        public static readonly string DownloadURL = "http://ttlrcct2.qianqian.com/dll/lyricsvr.dll?dl?Id={0}&Code={1}";
        private LRCBase selectSong = null;
        public LRCDownload()
        {

        }

        public LRCBase SelectSong
        {
            get
            {
                return this.selectSong;
            }
            set
            {
                this.selectSong = value;
            }
        }
        public List<LRCBase> DownloadLrcList(string song, string singer)
        {
            song = song.ToLower().Replace(" ", "").Replace("'", "");
            song = EncodeHelper.ToHexString(song, Encoding.Unicode);
            singer = singer.ToLower().Replace(" ", "").Replace("'", "");
            singer = EncodeHelper.ToHexString(singer, Encoding.Unicode);
            string url = string.Format(SearchURL, singer, song);
            string lrclist = RequestUrl(url);
            List<LRCBase> lrcbaselist = new List<LRCBase>();
            XmlDocument doc = new XmlDocument();
            try
            {
                doc.LoadXml(lrclist);
            }
            catch
            {
            }
            selectSong = null;
            if (doc != null)
            {
                XmlNodeList list = doc.SelectNodes("/result/lrc");
                if (list != null)
                {
                    for (int i = 0; i < list.Count; i++)
                    {
                        lrcbaselist.Add(new LRCBase((Convert.ToInt32(list.Item(i).Attributes["id"].Value)),
                            list.Item(i).Attributes["title"].Value, list.Item(i).Attributes["artist"].Value));
                    }
                }
            }
            if (lrcbaselist.Count > 0)
            {
                selectSong = lrcbaselist[0];
            }
            else
            {
                selectSong = null;
            }
            return lrcbaselist;
        }

        public string DownloadLrc(LRCBase lrcSong)
        {
            string retMes = "";
            if (selectSong == null)
            {
                retMes = "No lrc searched!";
                return retMes;
            }
            if (lrcSong != null)
            {
                selectSong = lrcSong;
            }
            string url = string.Format(DownloadURL, lrcSong.ID, EncodeHelper.CreateQianQianCode(lrcSong.Singer,
                lrcSong.SongName, lrcSong.ID));
            retMes = RequestUrl(url);
            return retMes;
        }

        private string RequestUrl(string url)
        {
            WebRequest request = WebRequest.Create(url);
            StringBuilder sb = new StringBuilder();
            try
            {
                using (StreamReader sr = new StreamReader(request.GetResponse().GetResponseStream()))
                {
                    string line = string.Empty;
                    //while ((line = sr.ReadLine()) != null)
                    {
                        sb.Append(sr.ReadToEnd());
                    }
                }
            }
            catch (WebException ex)
            {

            }
            return sb.ToString();
        }
    }
}
