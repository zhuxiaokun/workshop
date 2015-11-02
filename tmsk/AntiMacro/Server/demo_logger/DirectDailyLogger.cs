using System;
using System.IO;
using System.Diagnostics;

namespace LOGGER
{
    public struct LoggerLevel
    {
        public const int Log_Fatal = (1<<0);
        public const int Log_Error = (1<<1);
        public const int Log_Warn  = (1<<2);
        public const int Log_Debug = (1<<3);
        public const int Log_Info  = (1<<4);
        public const int Log_All   = 0x7fffffff;
    }
    public interface ILogger
    {
        void setLogLevel(int level);
        void setFlushLineCount(int lineCount);
        void log(int level, string fmt, params object[] args);
        void logAndFlush(int level, string fmt, params object[] args);
    }
    public class DirectFileLogger : ILogger, IDisposable
    {
        protected int m_logLevel = 0x7fffffff;
        protected int m_refreshLineCount = 0;
        protected int m_flushLineCount = 16;
        protected string m_filePath;
        protected FileStream m_outStream;
        protected StreamWriter m_streamWriter;

        public DirectFileLogger(string filePath)
        {
            m_filePath = filePath;
        }
        ~DirectFileLogger()
        {
            if (m_streamWriter != null)
            {
                try { m_streamWriter.Flush(); }
                catch (System.Exception ex) { }
                try { m_streamWriter.Close(); }
                catch (System.Exception ex) { }
                m_streamWriter = null;
                m_outStream = null;
            }
        }
        public void Dispose()
        {
            try { m_streamWriter.Flush(); } catch (System.Exception ex) { }
            try { m_streamWriter.Close(); } catch (System.Exception ex) { }
            m_streamWriter = null;
            m_outStream = null;
        }
        public void setLogLevel(int level)
        {
            m_logLevel = level;
        }
        public void setFlushLineCount(int lineCount)
        {
            if (lineCount < 1) lineCount = 1;
            m_flushLineCount = lineCount;
        }
        virtual protected StreamWriter getStreamWriter()
        {
            if(m_streamWriter != null) return m_streamWriter;
            Trace.Assert(m_outStream == null);

            string dir = Path.GetDirectoryName(m_filePath);
            if (!Directory.Exists(dir))
            {
                try
                {
                    DirectoryInfo dinfo = Directory.CreateDirectory(dir);
                    if (!dinfo.Exists)
                        return null;
                }
                catch (System.Exception ex)
                {
                    return null;
                }
            }
            try
            {
                m_outStream = new FileStream(m_filePath, FileMode.Append, FileAccess.Write);
                m_streamWriter = new StreamWriter(m_outStream);
            }
            catch (System.Exception ex)
            {
                if (m_outStream != null)
                {
                    m_outStream.Close();
                    m_outStream = null;
                }
                return null;
            }
            return m_streamWriter;
        }
        virtual public void log(int level, string fmt, params object[] args)
        {
            if((m_logLevel & level) != 0)
            {
                StreamWriter writer = this.getStreamWriter();
                if (writer != null)
                {
                    DateTime now = DateTime.Now;
                    int n1 = now.Year * 10000 + now.Month * 100 + now.Day;
                    int n2 = now.Hour * 10000 + now.Minute * 100 + now.Second;
                    m_streamWriter.Write("{0:0000}-{1:00}-{2:00} {3:00}:{4:00}:{5:00} ",
                        now.Year, now.Month, now.Day, now.Hour, now.Minute, now.Second);
                    m_streamWriter.Write(fmt, args);
                    m_streamWriter.Write('\n');
                    m_refreshLineCount += 1;
                    if (m_refreshLineCount >= m_flushLineCount)
                    {
                        writer.Flush();
                        m_refreshLineCount = 0;
                    }
                }
            }
        }
        virtual public void logAndFlush(int level, string fmt, params object[] args)
        {
            if ((m_logLevel & level) != 0)
            {
                StreamWriter writer = this.getStreamWriter();
                if (writer != null)
                {
                    m_streamWriter.WriteLine(fmt, args);
                    writer.Flush();
                    m_refreshLineCount = 0;
                }
            }
        }
    }
    public class DirectFileLogger_MT : DirectFileLogger
    {
        private object m_locker = new object();
        public DirectFileLogger_MT(string filepath)
            : base(filepath)
        {
            
        }
        override public void log(int level, string fmt, params object[] args)
        {
            if ((m_logLevel & level) != 0)
            {
                lock (m_locker)
                {
                    base.log(level, fmt, args);
                }
            }
        }
        override public void logAndFlush(int level, string fmt, params object[] args)
        {
            if ((m_logLevel & level) != 0)
            {
                lock (m_locker)
                {
                    base.logAndFlush(level, fmt, args);
                }
            }
        }
    }
    public class DirectDailyLogger : DirectFileLogger
    {
        private string m_dirName;
        private string m_baseName;
        private string m_extName;
        private DateTime m_expireTime;
        public DirectDailyLogger(string filePath):base(filePath)
        {
            m_dirName = Path.GetDirectoryName(filePath);
            m_baseName = Path.GetFileNameWithoutExtension(filePath);
            m_extName = Path.GetExtension(filePath);
        }
        override protected StreamWriter getStreamWriter()
        {
            if (m_streamWriter != null)
            {
                Trace.Assert(m_outStream != null);
                if(DateTime.Now < m_expireTime)
                    return m_streamWriter;
                m_streamWriter.Flush();
                m_streamWriter.Close();
                m_streamWriter = null;
                m_outStream = null;
                m_refreshLineCount = 0;
            }

            Trace.Assert(m_outStream == null);
            int yyyymmdd = DateTime.Now.Year * 10000 + DateTime.Now.Month * 100 + DateTime.Now.Day;
            int hhmmss = DateTime.Now.Hour * 10000 + DateTime.Now.Minute * 100 + DateTime.Now.Second;

            m_expireTime = DateTime.Now;
            m_expireTime.AddDays(1);
            m_expireTime.Subtract(DateTime.Now.TimeOfDay);

            string filePath = m_dirName
                + Path.DirectorySeparatorChar
                + m_baseName
                + '-'
                + string.Format("{0,8}", yyyymmdd)
                + m_extName;

            if (!Directory.Exists(m_dirName))
            {
                try
                {
                    DirectoryInfo dinfo = Directory.CreateDirectory(m_dirName);
                    if (dinfo.Exists)
                        return null;
                }
                catch (System.Exception ex)
                {
                    return null;
                }
            }
            try
            {
                m_outStream = new FileStream(filePath, FileMode.Append, FileAccess.Write);
                m_streamWriter = new StreamWriter(m_outStream);
            }
            catch (System.Exception ex)
            {
                if (m_outStream != null)
                {
                    m_outStream.Close();
                    m_outStream = null;
                }
                return null;
            }
            return m_streamWriter;
        }
    }
    public class DirectDailyLogger_MT : DirectDailyLogger
    {
        private object m_locker = new object();
        public DirectDailyLogger_MT(string filepath)
            : base(filepath)
        {

        }
        override public void log(int level, string fmt, params object[] args)
        {
            if ((m_logLevel & level) != 0)
            {
                lock (m_locker)
                {
                    base.log(level, fmt, args);
                }
            }
        }
        override public void logAndFlush(int level, string fmt, params object[] args)
        {
            if ((m_logLevel & level) != 0)
            {
                lock (m_locker)
                {
                    base.logAndFlush(level, fmt, args);
                }
            }
        }
    }
}
