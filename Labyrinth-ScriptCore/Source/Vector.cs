using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace Labyrinth
{
    public struct Vector3
    {
        public float X, Y, Z;

        public static Vector3 Zero => new Vector3(0.0f);

        public Vector3(float x, float y, float z)
        {
            this.X = x;
            this.Y = y;
            this.Z = z;
        }
        public Vector3(float scalar)
        {
            this.X = scalar;
            this.Y = scalar;
            this.Z = scalar;
        }

        public static Vector3 operator*(Vector3 vector, float scalar)
        {
            return new Vector3(vector.X * scalar, vector.Y * scalar, vector.Z * scalar);
        }
        public static Vector3 operator+(Vector3 a, Vector3 b)
        {
            return new Vector3(a.X + b.X, b.Y + b.Y, a.Z + b.Z);
        }
    }
}
