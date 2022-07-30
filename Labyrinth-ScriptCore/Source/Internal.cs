using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Runtime.CompilerServices;

namespace Labyrinth
{
    public static class Internal
    {
        #region Logging
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Trace(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Info(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Warning(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Error(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Log_Critical(string text);
        #endregion

        #region Input
        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static bool Input_IsKeyPressed(KeyCode key);
        #endregion

        #region Entity

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetTranslation(ulong entUUID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetTranslation(ulong entUUID, ref Vector3 translation);

        #endregion
    }
}
