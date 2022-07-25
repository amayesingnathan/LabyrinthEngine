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

        public override void OnUpdate()
        {
            Internal.LogInfo("Player.OnUpdate");
        }
    }
}
