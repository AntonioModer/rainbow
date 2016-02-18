using System;
using System.Diagnostics;
using System.IO;
using System.Runtime.CompilerServices;

namespace Rainbow.Utils
{
    public sealed class PerformanceTimer : IDisposable
    {
        private readonly Stopwatch stopwatch;
        private readonly string caller;

        public PerformanceTimer(
            string caller = null,
            [CallerMemberName] string callerMemberName = "",
            [CallerFilePath] string callerFilePath = "")
        {
            this.caller = string.IsNullOrEmpty(caller)
                ? InduceCaller(callerMemberName, callerFilePath)
                : caller;
            stopwatch = Stopwatch.StartNew();
        }

        public void Dispose()
        {
            stopwatch.Stop();
            Console.WriteLine($"{caller} finished in {stopwatch.ElapsedMilliseconds} ms");
        }

        private static string InduceCaller(string callerMemberName, string callerFilePath)
        {
            string inducedClass = Path.GetFileNameWithoutExtension(callerFilePath);
            return $"{inducedClass}.{callerMemberName}";
        }
    }
}
