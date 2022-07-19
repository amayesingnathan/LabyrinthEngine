using System;

namespace Labyrinth
{
    public class Entity
    {
        public float FloatVar { get; set; }

        public Entity()
        {
            Console.WriteLine("Main constructor");
            Internal.LogInfo("Main constructor");
        }
        public Entity(float x)
        {
            FloatVar = x;
            Console.WriteLine($"Main constructor + {FloatVar}");
            Internal.LogInfo($"Main constructor + {FloatVar}");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World from C#!");
            Internal.LogInfo("Hello World from C#!");
        }
        public void PrintMessage(float x)
        {
            Console.WriteLine($"Hello World from C#! + {x}");
            Internal.LogInfo($"Hello World from C# + {x}!");
        }
        public void PrintMessageCustom(string message)
        {
            Console.WriteLine($"C# says: {message}");
            Internal.LogInfo($"C# says: {message}");
        }

        public void PrintVariables(float a, float b)
        {
            Console.WriteLine($"Float 1: {a}\tFloat 2: {b}");
            Internal.LogInfo($"Float 1: {a}\tFloat 2: {b}");
        }
    }
}