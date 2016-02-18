using System;

using Rainbow.Graphics;

namespace Rainbow
{
    public sealed class Program
    {
        public static void Main(string[] args)
        {
            // Not really needed but nice to have in case you need to debug .NET
            // DLLs.
        }

        public static unsafe void Initialize(int width, int height)
        {
            SpriteBatch.InitializeComponents();
            Console.WriteLine($"Rainbow#: Initialize (surface size: {width}x{height})");
        }

        public static unsafe void Update(ulong dt)
        {
            //Console.WriteLine($"Rainbow#: Update (dt: {dt})");
        }

        public static unsafe void OnMemoryWarning()
        {
            Console.WriteLine($"Rainbow#: OnMemoryWarning");
        }
    }
}
