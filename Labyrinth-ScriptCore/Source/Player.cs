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

            Vector3 velocity = Vector3.Zero;
            if (Input.IsKeyPressed(KeyCode.A))
            {
                velocity.X = -1.0f;
            }
            else if (Input.IsKeyPressed(KeyCode.D))
            {
                velocity.X = 1.0f;
            }
            if (Input.IsKeyPressed(KeyCode.W))
            {
                velocity.Y = 1.0f;
            }
            else if (Input.IsKeyPressed(KeyCode.S))
            {
                velocity.Y = -1.0f;
            }

            float speed = 3.0f;
            velocity *= speed;

            //Log.Info($"From C++:  X: {Translation.X}\tY: {Translation.Y}\tZ: {Translation.Z}");
            //Log.Info($"From C# :  X: {translation.X}\tY: {translation.Y}\tZ: {translation.Z}");
            //Log.Info($"Vx: {velocity.X}\tVy: {velocity.Y}");

            if (!velocity.IsZero())
            {
                Vector3 translation = Translation;
                translation += velocity * ts;
                Translation = translation;
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
