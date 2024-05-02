using System;
using System.Collections.Generic;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Threading;

namespace CE
{
    [AttributeUsage(AttributeTargets.Class, AllowMultiple = false, Inherited = false)]
    public class NativeClassAttribute : System.Attribute
    {
        public NativeClassAttribute(string fullClassName)
        {
            FullClassName = fullClassName;
        }

        public string FullClassName { get; private set; }
    }

    public class ExportAttribute : System.Attribute 
    {
        public ExportAttribute()
        {

        }
    }

    [NativeClass("/Code/Core.CE::Object")]
    public class Object
    {
        [NonSerialized]
        private IntPtr _impl = IntPtr.Zero;
        [NonSerialized]
        private string fullClassName = "";

        public IntPtr NativePtr => _impl;

        public static T Create<T>(string name, Object outer) where T : Object
        {
            return Create<T>(name, outer, typeof(T));
        }

        public static T Create<T>(string name, Object outer, Type classType) where T : Object
        {
            if (!classType.IsClass)
                return null;
            if (classType != typeof(T) && !classType.IsSubclassOf(typeof(T)))
                return null;
            
            return Activator.CreateInstance(classType) as T;
        }

        public static T Create<T>(string name) where T : Object
        {
            return null;
        }

        protected Object(IntPtr impl)
        {
            _impl = impl;

            NativeClassAttribute nativeClassAttribute = GetType().GetCustomAttribute<NativeClassAttribute>();
            if (nativeClassAttribute != null)
            {
                fullClassName = nativeClassAttribute.FullClassName;
            }
        }

        public Object(Object outer = null, string name = "Object")
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
            if (_impl != IntPtr.Zero)
            {
                Destroy();
            }
        }

        public void Destroy()
        {
            DestroyObjectImpl(_impl);
            _impl = IntPtr.Zero;
        }

        public static implicit operator bool(Object obj)
        {
            return obj != null && obj.IsValid();
        }

        public static bool operator==(Object lhs, Object rhs)
        {
            if (lhs == null && rhs == null)
                return true;
            if (lhs == null || rhs == null)
                return false;

            return lhs.NativePtr == rhs.NativePtr;
        }

        public static bool operator!=(Object lhs, Object rhs)
        {
            return !(lhs == rhs);
        }

        public override bool Equals(object obj)
        {
            return obj is Object @object && (this == @object);
        }

        public override int GetHashCode()
        {
            return _impl.GetHashCode();
        }

        public bool IsValid()
        {
            return _impl != IntPtr.Zero;
        }

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr CreateObjectImpl(string fullClassTypeName, string objectName, Object outer = null);

        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern void DestroyObjectImpl(IntPtr impl);
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr GetTransientPackageImpl();
        [MethodImpl(MethodImplOptions.InternalCall)]
        private static extern IntPtr GetTransientPackageImpl(string moduleName);

        public static Package GetTransientPackage()
        {
            return new Package(GetTransientPackageImpl());
        }

        void Test()
        {
            Object obj = Object.Create<Package>("MyObject", GetTransientPackage());
        }
    }
}
