using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Threading;
using System.Diagnostics;

namespace asynlogger
{
    class Program
    {
        static void Main(string[] args)
        {
            LOGGER.DirectDailyLogger_MT logger = new LOGGER.DirectDailyLogger_MT("d:\\tmp\\a\\debug.log");
            logger.setLogLevel(LOGGER.LoggerLevel.Log_All);
            logger.setFlushLineCount(8);

            int i = 1;
            System.Random rand = new System.Random();
            while (true)
            {
                logger.log(LOGGER.LoggerLevel.Log_All, "[{0}] hello world {1} !", i++, "zhuxiaokun");
                System.Threading.Thread.Sleep(rand.Next(0, 1000));
            }
            logger.Dispose();
        }
    }
}
