using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Net;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace WebControlLED
{
    class LEDControl
    {
        private static string ServerHandle = "http://";
        private string ServerAddress;

        public LEDControl(string address)
        {
            ServerAddress = address;
        }

        /// <summary>
        /// 彩虹模式开启关闭
        /// </summary>
        /// <param name="lightSwitch"></param>
        public void SetMode(bool lightMode)
        {
            StringBuilder requestsStr = new StringBuilder(ServerHandle);
            requestsStr.Append(ServerAddress);
            if (lightMode)
            {
                requestsStr.Append(@"/mode?mode=auto");
            }
            else
            {
                requestsStr.Append(@"/mode?mode=noauto");
            }
            Get(requestsStr.ToString());
            
        }
        /// <summary>
        /// 主开关控制
        /// </summary>
        /// <param name="lightSwitch"></param>
        public void SetSwitch(bool lightSwitch)
        {
            StringBuilder requestsStr = new StringBuilder(ServerHandle);
            requestsStr.Append(ServerAddress);
            if (lightSwitch)
            {
                requestsStr.Append(@"/switch?light=on");
            }
            else
            {
                requestsStr.Append(@"/switch?light=off");
            }
            Get(requestsStr.ToString());
        }
        public void SetColor(int r, int g, int b)
        {
            StringBuilder requests = new StringBuilder(ServerHandle);
            requests.Append(ServerAddress);
            requests.Append(@"/color?R=");
            requests.Append(r);
            requests.Append(@"&G=");
            requests.Append(g);
            requests.Append(@"&B=");
            requests.Append(b);
            Get(requests.ToString());
        }

        public void SetBright(int bright)
        {
            StringBuilder requests = new StringBuilder(ServerHandle);
            requests.Append(ServerAddress);
            requests.Append(@"/bright?bright=");
            requests.Append(bright);
            Get(requests.ToString());
        }

        /// <summary>
        /// 启用一个线程去访问控制板
        /// </summary>
        /// <param name="url"></param>
        /// <returns></returns>
        private static void Get(string url)
        {
            //string result = "";
            new Thread(new ThreadStart(new Action(() =>
            {
                HttpWebRequest req = (HttpWebRequest)WebRequest.Create(url);
                HttpWebResponse resp = (HttpWebResponse)req.GetResponse();
                if (resp.StatusCode == HttpStatusCode.OK)
                {
                    Stream stream = resp.GetResponseStream();
                    try
                    {
                        //获取内容
                        using (StreamReader reader = new StreamReader(stream))
                        {
                            //result = reader.ReadToEnd();
                        }
                    }
                    finally
                    {
                        stream.Close();
                    }
                }
            }))).Start();
            
            //return result;
        }
    }
}
