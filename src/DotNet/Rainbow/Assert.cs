using System;
using System.Diagnostics;

namespace Rainbow
{
    public static class Assert
    {
        private const string DEBUG = @"DEBUG";
        private const string ValuesCannotBeEqual = "Values cannot be equal.";
        private const string ValuesMustBeEqual = "Values must be equal";

        [Conditional(DEBUG)]
        public static void Equal(int lhs, int rhs)
        {
            Debug.Assert(IsEqual(lhs, rhs), $"{ValuesMustBeEqual}: {lhs} vs {rhs}");
        }

        [Conditional(DEBUG)]
        public static void Equal(double lhs, double rhs)
        {
            Debug.Assert(IsEqual(lhs, rhs), $"{ValuesMustBeEqual}: {lhs} vs {rhs}");
        }

        [Conditional(DEBUG)]
        public static void Equal(string lhs, string rhs)
        {
            Debug.Assert(IsEqual(lhs, rhs), $"{ValuesMustBeEqual}: \"{lhs}\" vs \"{rhs}\"");
        }

        [Conditional(DEBUG)]
        public static void Equal<T>(T lhs, T rhs) where T : class
        {
            Debug.Assert(IsEqual(lhs, rhs), $"{ValuesMustBeEqual}.");
        }

        [Conditional(DEBUG)]
        public static void False(bool condition, string message)
        {
            Debug.Assert(!condition, message);
        }

        [Conditional(DEBUG)]
        public static void NotEqual(int lhs, int rhs)
        {
            Debug.Assert(!IsEqual(lhs, rhs), ValuesCannotBeEqual);
        }

        [Conditional(DEBUG)]
        public static void NotEqual(double lhs, double rhs)
        {
            Debug.Assert(!IsEqual(lhs, rhs), ValuesCannotBeEqual);
        }

        [Conditional(DEBUG)]
        public static void NotEqual(string lhs, string rhs)
        {
            Debug.Assert(!IsEqual(lhs, rhs), ValuesCannotBeEqual);
        }

        [Conditional(DEBUG)]
        public static void NotEqual<T>(T lhs, T rhs) where T : class
        {
            Debug.Assert(!IsEqual(lhs, rhs), ValuesCannotBeEqual);
        }

        [Conditional(DEBUG)]
        public static void NotNull<T>(T value)
        {
            Debug.Assert(value != null);
        }

        [Conditional(DEBUG)]
        public static void Null<T>(T value)
        {
            Debug.Assert(value == null, "Value must be null.");
        }

        [Conditional(DEBUG)]
        public static void True(bool condition, string message)
        {
            Debug.Assert(condition, message);
        }

        private static bool IsEqual(int lhs, int rhs)
        {
            return lhs == rhs;
        }

        private static bool IsEqual(double lhs, double rhs)
        {
            const double DoubleTolerance = 0.00001;
            return Math.Abs(lhs - rhs) < DoubleTolerance;
        }

        private static bool IsEqual(string lhs, string rhs)
        {
            return string.IsNullOrEmpty(lhs) ? string.IsNullOrEmpty(rhs) : lhs.Equals(rhs);
        }

        private static bool IsEqual<T>(T lhs, T rhs) where T : class
        {
            return lhs == null ? rhs == null : lhs.Equals(rhs);
        }
    }
}
