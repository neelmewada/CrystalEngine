using System;
using System.CodeDom;
using System.CodeDom.Compiler;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace CE
{

    // Native ClassPath: /Code/ScriptCore.CE::CustomObject
    public class CustomObject : Object
    {
        [Export]
        public string textValue = "";

        public CustomObject() 
        {
            
        }
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

