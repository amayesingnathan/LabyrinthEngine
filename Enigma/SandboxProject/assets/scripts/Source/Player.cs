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
        public float MaxSpeed;
        public float Timestamp;
        public Vector3 PlayerTranslation;
        public bool Initialised;
        public override void OnCreate()
        {
            Log.Info($"Player.OnCreate {ID}");
            MaxSpeed = 1.5f;
            PlayerTranslation = Translation;
            Initialised = true;
            Timestamp = 0.0f;
        }

        public override void OnUpdate(float ts)
        {
            //Log.Info($"Player.OnUpdate: {ts}");

            Timestamp += ts;
            PlayerTranslation = Translation;

            Vector2 jump = Vector2.Zero;
            Vector2 move = Vector2.Zero;
            if (Input.IsKeyPressed(KeyCode.A))
            {
                move.X = -0.5f;
            }
            if (Input.IsKeyPressed(KeyCode.D))
            {
                move.X = 0.5f;
            }

            if (Input.IsKeyPressed(KeyCode.W))
            {
                jump.Y = 3.0f;
            }

            RigidBodyComponent rigidBody = GetComponent<RigidBodyComponent>();
            Vector2 vel = rigidBody.LinearVelocity;

            if (!move.IsZero() && Math.Abs(vel.X) <= MaxSpeed)
            {
                move *= ts;
                rigidBody.ApplyLinearImpulse(move, Vector2.Zero, true);
            }
            if (vel.Y <= 0.0f && vel.Y >= -0.001f && !jump.IsZero())
                rigidBody.AddForce(jump, Vector2.Zero, true);
        }
    }
}
