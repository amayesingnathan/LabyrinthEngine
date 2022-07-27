using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

using Labyrinth;

namespace Sandbox
{
    public class Player : Entity
    {
        public override void OnCreate()
        {
            Internal.LogInfo("Player.OnCreate");
        }

        public override void OnUpdate(float ts)
        {
            Internal.LogInfo($"Player.OnUpdate: {ts}");
        }
    }
    public class Player2 : Entity
    {
        public override void OnCreate()
        {
            Internal.LogInfo("Player2.OnCreate");
        }

        public override void OnUpdate(float ts)
        {
            Internal.LogInfo($"Player2.OnUpdate: {ts}");
        }
    }
}
