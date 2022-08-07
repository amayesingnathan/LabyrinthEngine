using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
    public static class Log
    {
        public static void Trace(string text) { Internal.Log_Trace(text); }
        public static void Info(string text) { Internal.Log_Info(text); }
        public static void Warning(string text) { Internal.Log_Warning(text); }
        public static void Error(string text) { Internal.Log_Error(text); }
        public static void Critical(string text) { Internal.Log_Critical(text); }
    }
}
