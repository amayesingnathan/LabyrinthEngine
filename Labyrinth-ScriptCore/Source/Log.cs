using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
    internal static class Log
    {
        internal static void Trace(string text) { Internal.Log_Trace(text); }
        internal static void Info(string text) { Internal.Log_Info(text); }
        internal static void Warning(string text) { Internal.Log_Warning(text); }
        internal static void Error(string text) { Internal.Log_Error(text); }
        internal static void Critical(string text) { Internal.Log_Critical(text); }
    }
}
