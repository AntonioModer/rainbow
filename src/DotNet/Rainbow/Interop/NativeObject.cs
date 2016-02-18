using System;
using System.Runtime.InteropServices;

namespace Rainbow.Interop
{
    public abstract class NativeObject : IDisposable
    {
        /// <summary>
        /// Handle for the unmanaged object on which all operations are done.
        /// </summary>
        protected IntPtr NativeHandle { get; set; }

        /// <summary>
        /// Called when managed state (managed objects) should be disposed.
        /// </summary>
        protected virtual void OnDisposing() { }

        #region IDisposable Support
        private bool isDisposed = false;  // To detect redundant calls.

        ~NativeObject()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) below.
            Dispose(false);
        }

        public void Dispose()
        {
            // Do not change this code. Put cleanup code in Dispose(bool disposing) below.
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        private void Dispose(bool disposing)
        {
            if (!isDisposed)
            {
                if (disposing)
                {
                    OnDisposing();
                }

                // TODO: free unmanaged resources (unmanaged objects).
                NativeHandle = IntPtr.Zero;
                isDisposed = true;
            }
        }
        #endregion

        #region Rainbow.Interop Helpers
        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        protected unsafe delegate T GetDelegate<T>(IntPtr nativeHandle);

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        protected unsafe delegate TResult GetDelegate<T1, T2, TResult>(IntPtr nativeHandle, T1 param1, T2 param2);

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        protected unsafe delegate void SetDelegate<T>(IntPtr nativeHandle, T value);

        [UnmanagedFunctionPointer(CallingConvention.ThisCall)]
        protected unsafe delegate void SetDelegate<T1, T2>(IntPtr nativeHandle, T1 value1, T2 value2);

        protected static GetDelegate<T> GetDelegateForFunctionPointer<T>(IntPtr funcPtr)
        {
            Assert.NotNull(funcPtr);
            return Marshal.GetDelegateForFunctionPointer<GetDelegate<T>>(funcPtr);
        }

        protected static GetDelegate<T1, T2, TResult> GetDelegateForFunctionPointer<T1, T2, TResult>(IntPtr funcPtr)
        {
            Assert.NotNull(funcPtr);
            return Marshal.GetDelegateForFunctionPointer<GetDelegate<T1, T2, TResult>>(funcPtr);
        }

        protected static SetDelegate<T> SetDelegateForFunctionPointer<T>(IntPtr funcPtr)
        {
            Assert.NotNull(funcPtr);
            return Marshal.GetDelegateForFunctionPointer<SetDelegate<T>>(funcPtr);
        }

        protected static SetDelegate<T1, T2> SetDelegateForFunctionPointer<T1, T2>(IntPtr funcPtr)
        {
            Assert.NotNull(funcPtr);
            return Marshal.GetDelegateForFunctionPointer<SetDelegate<T1, T2>>(funcPtr);
        }
        #endregion
    }
}
