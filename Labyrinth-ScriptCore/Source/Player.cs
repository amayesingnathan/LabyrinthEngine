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
            Log.Info($"Player.OnCreate {ID}");
        }

        public override void OnUpdate(float ts)
        {
            //Log.Info($"Player.OnUpdate: {ts}");

            Vector3 force = Vector3.Zero;
            if (Input.IsKeyPressed(KeyCode.A))
            {
                force.X = -1.0f;
            }
            else if (Input.IsKeyPressed(KeyCode.D))
            {
                force.X = 1.0f;
            }
            if (Input.IsKeyPressed(KeyCode.W))
            {
                force.Y = 1.0f;
            }
            else if (Input.IsKeyPressed(KeyCode.S))
            {
                force.Y = -1.0f;
            }

            if (!force.IsZero())
            {
                Translation += force * ts;
                //force = force * ts;
                //RigidBodyComponent rigidBody = GetComponent<RigidBodyComponent>();
                //rigidBody.ApplyLinearImpulse(force, Vector2.Zero, true);
            }
        }   
    }
    public class Player2 : Entity
    {
        public Player2(ulong UUID) : base(UUID) { }
        public override void OnCreate()
        {
            Log.Info($"Player2.OnCreate {ID}");
        }

        public override void OnUpdate(float ts)
        {
            Log.Info($"Player2.OnUpdate: {ts}");
        }
    }
}
