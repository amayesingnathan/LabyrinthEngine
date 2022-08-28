using Labyrinth;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Sandbox
{
    public class Tunnel : Entity
    {
        public override void OnCreate()
        {
            Log.Info($"Tunnel.OnCreate {ID}");
        }
        public override void OnUpdate(float ts)
        {

        }
    }
}
