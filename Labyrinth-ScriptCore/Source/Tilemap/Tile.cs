using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
    public class Tile : Entity
    {
        public override void OnCreate()
        {
            Log.Info($"Tile.OnCreate {ID}");
        }

        public override void OnUpdate(float ts)
        {
        }
    }
}
