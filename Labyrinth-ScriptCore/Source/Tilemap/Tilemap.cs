using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
    public class Tilemap : Entity
    {
        public override void OnCreate()
        {
            Log.Info($"Tilemap.OnCreate {ID}");
        }

        public override void OnUpdate(float ts)
        {
        }
    }
}
