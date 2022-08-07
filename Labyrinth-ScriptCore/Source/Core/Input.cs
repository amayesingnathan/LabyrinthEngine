using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
    public static class Input
    {
        public static bool IsKeyPressed(KeyCode keycode) { return Internal.Input_IsKeyPressed(keycode); }
    }
}
