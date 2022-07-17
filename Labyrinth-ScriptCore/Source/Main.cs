using System;
using System.Runtime.CompilerServices;

namespace Labyrinth
{
    public class TestClass
    {
        public float FloatVar { get; set; }

        public TestClass()
        {
            Console.WriteLine("Main constructor");
        }

        public void PrintMessage()
        {
            Console.WriteLine("Hello World from C#!");
        }
        public void PrintMessageCustom(string message)
        {
            Console.WriteLine($"C# says: {message}");
        }

        public void PrintVariables(float a, float b)
        {
            Console.WriteLine($"Float 1: {a}\tFloat 2: {b}");
        }
    }
}