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
        public float JumpCD { get; set; }
        public override void OnCreate()
        {
            Log.Info($"Player.OnCreate {ID}");
            JumpCD = 0.0f;
        }

        public override void OnUpdate(float ts)
        {
            //Log.Info($"Player.OnUpdate: {ts}");

            bool jumping = false;
            Vector2 jump = Vector2.Zero;
            Vector2 move = Vector2.Zero;
            if (Input.IsKeyPressed(KeyCode.A))
            {
                move.X = -10.0f;
            }
            else if (Input.IsKeyPressed(KeyCode.D))
            {
                move.X = 10.0f;
            }
            if (Input.IsKeyPressed(KeyCode.W))
            {
                jump.Y = 150.0f;
            }

            if (JumpCD > 0.0f)
                JumpCD -= ts;

            RigidBodyComponent rigidBody = GetComponent<RigidBodyComponent>();
            TransformComponent transform = GetComponent<TransformComponent>();

            if (transform.Translation.Y <= 0.01f)
                jumping = false;
            else
                jumping = true;

            if (!move.IsZero() && !jumping)
            {
                move *= ts;
                rigidBody.ApplyLinearImpulse(move, Vector2.Zero, true);
            }
            if (JumpCD <= 0.0f && !jump.IsZero())
            {
                rigidBody.AddForce(jump, Vector2.Zero, true);
                JumpCD = 1.0f;
            }
        }
    }
}
