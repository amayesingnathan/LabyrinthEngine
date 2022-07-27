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
        internal extern static void LogTrace(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogInfo(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogWarning(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogError(string text);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void LogCritical(string text);
        #endregion // Logging

        #region Components

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_GetTranslation(ulong entUUID, out Vector3 translation);

        [MethodImplAttribute(MethodImplOptions.InternalCall)]
        internal extern static void Entity_SetTranslation(ulong entUUID, ref Vector3 translation);

        #endregion
    }
}
