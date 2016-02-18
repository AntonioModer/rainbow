using System;

using Rainbow.Interop;

namespace Rainbow.Graphics
{
    public sealed class SpriteBatch : NativeObject
    {
        public bool IsVisible
        {
            get { return isVisibleDelegate(NativeHandle); }
            set { setVisibleDelegate(NativeHandle, value); }
        }

        public object CreateSprite(int width, int height)
        {
            return createSpriteDelegate(NativeHandle, width, height);
        }

        public void Move(float x, float y)
        {
            moveDelegate(NativeHandle, x, y);
        }

        #region Rainbow.Interop
        private static bool isInitialized = false;
        private static GetDelegate<bool> isVisibleDelegate;
        private static SetDelegate<bool> setVisibleDelegate;
        private static GetDelegate<int, int, object> createSpriteDelegate;
        private static SetDelegate<float, float> moveDelegate;

        internal static void InitializeComponents()
        {
            Assert.False(isInitialized, $"{typeof(SpriteBatch)} was already initialized");

            isVisibleDelegate = GetDelegateForFunctionPointer<bool>(IntPtr.Zero);
            setVisibleDelegate = SetDelegateForFunctionPointer<bool>(IntPtr.Zero);
            createSpriteDelegate = GetDelegateForFunctionPointer<int, int, object>(IntPtr.Zero);
            moveDelegate = SetDelegateForFunctionPointer<float, float>(IntPtr.Zero);
            isInitialized = true;
        }
        #endregion
    }
}
