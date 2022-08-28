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
        public const float MaxSpeed = 1.5f;
        public override void OnCreate()
        {
            Log.Info($"Player.OnCreate {ID}");
        }

        public override void OnUpdate(float ts)
        {
            //Log.Info($"Player.OnUpdate: {ts}");

            Vector2 jump = Vector2.Zero;
            Vector2 move = Vector2.Zero;
            if (Input.IsKeyPressed(KeyCode.A))
            {
                move.X = -1.0f;
            }
            else if (Input.IsKeyPressed(KeyCode.D))
            {
                move.X = 1.0f;
            }

            if (Input.IsKeyPressed(KeyCode.W))
            {
                jump.Y = 5.0f;
            }

            RigidBodyComponent rigidBody = GetComponent<RigidBodyComponent>();
            Vector2 vel = rigidBody.LinearVelocity;

            if (!move.IsZero() && Math.Abs(vel.X) <= MaxSpeed)
            {
                move *= ts;
                rigidBody.ApplyLinearImpulse(move, Vector2.Zero, true);
            }
            if (vel.Y <= 0.0f && vel.Y >= -0.1f && !jump.IsZero())
                rigidBody.AddForce(jump, Vector2.Zero, true);
        }
    }
}
