using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace CE
{
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = true)]
    public class NativeClassAttribute : System.Attribute
    {
        public NativeClassAttribute(string fullClassName)
        {
            FullClassName = fullClassName;
        }

        public string FullClassName { get; private set; }
    }

    [NativeClass("/Code/Core.CE::Object")]
    public class Object
    {
        [NonSerialized]
        private IntPtr _impl = IntPtr.Zero;
        [NonSerialized]
        private string fullClassName = "";

        public IntPtr NativePtr => _impl;

        public Object(string name = "Object", Object outer = null)
        {
            NativeClassAttribute nativeClassAttribute = GetType().GetCustomAttribute<NativeClassAttribute>();
            if (nativeClassAttribute != null)
            {
                fullClassName = nativeClassAttribute.FullClassName;
                _impl = CreateObjectImpl(fullClassName, name, outer);
            }
        }

        ~Object()
        {
            Destroy();
        }

        public void Destroy()
        {
            DestroyObjectImpl(_impl);
            _impl = IntPtr.Zero;
        }

        public bool IsValid()
        {
            return _impl != IntPtr.Zero;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr CreateObjectImpl(string fullClassTypeName, string objectName, Object outer = null);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void DestroyObjectImpl(IntPtr impl);

    }

    [NativeClass("/Code/Core.CE::Package")]
    public class Package : Object
    {

    }

    public class SampleClass
    {

        public static string TestMethod(string str)
        {
            return "From C#: " + GetCppString() + " | " + str;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern string GetCppString();

        public static void ObjectTest()
        {
            Object myObj = new Object();
            myObj = null;
        }
    }
}

