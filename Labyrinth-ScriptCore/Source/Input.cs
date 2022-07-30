using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
    internal static class Input
    {
        internal static bool IsKeyPressed(KeyCode keycode) { return Internal.Input_IsKeyPressed(keycode); }
    }
}
