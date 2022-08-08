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

            Vector2 force = Vector2.Zero;
            if (Input.IsKeyPressed(KeyCode.A))
            {
                force.X = -40.0f;
            }
            else if (Input.IsKeyPressed(KeyCode.D))
            {
                force.X = 40.0f;
            }
            if (Input.IsKeyPressed(KeyCode.W))
            {
                force.Y = 40.0f;
            }
            else if (Input.IsKeyPressed(KeyCode.S))
            {
                force.Y = -40.0f;
            }

            if (!force.IsZero())
            {
                force *= ts;
                RigidBodyComponent rigidBody = GetComponent<RigidBodyComponent>();
                rigidBody.ApplyLinearImpulse(force, Vector2.Zero, true);
            }
        }
    }
}
