using System;
using System.Linq;
using System.Reflection;
using System.Runtime.CompilerServices;

namespace CE
{
    
    public static class RttiHelper
    {

        public static FieldInfo[] GetFields(string assemblyQualifiedName)
        {
            Type type = Type.GetType(assemblyQualifiedName);

            if (type == null)
                return null;
            
            return type.GetFields(BindingFlags.Instance | BindingFlags.Public | BindingFlags.NonPublic)
                .Where(x => x.CustomAttributes.Any(c => c.AttributeType == typeof(ExportAttribute))).ToArray();
        }
    }

}