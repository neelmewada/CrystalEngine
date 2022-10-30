
#include "Object/Types.h"


namespace CE
{

    // ******************************
    // Class Type

    HashMap<String, ClassType*> ClassType::RegisteredClasses{};

    ClassType* ClassType::FindClass(String classPath)
    {
        if (classPath.IsEmpty())
            return nullptr;

        if (!RegisteredClasses.KeyExists(classPath))
            return nullptr;

        return RegisteredClasses[classPath];
    }

    void ClassType::RegisterClassType(ClassType* classType)
    {
        if (classType == nullptr)
            return;

        String classPath = classType->GetUnderlyingTypePath();
        if (!RegisteredClasses.KeyExists(classPath))
        {
            RegisteredClasses.Add({ classPath, classType });
        }
    }

    void ClassType::UnregisterClassType(ClassType* classType)
    {
        if (classType == nullptr)
            return;

        String classPath = classType->GetUnderlyingTypePath();
        RegisteredClasses.Remove(classPath);
    }

    // ******************************
    // Struct Type

    StructType::~StructType()
    {
        for (auto prop : Properties) // Cleanup properties
        {
            delete prop;
        }

        for (auto func : Functions) // Cleanup functions
        {
            delete func;
        }
    }

    u32 StructType::GetPropertyCount()
    {
        if (IsFunctionType())
            return 0;
        return Properties.GetSize();
    }

    PropertyType* StructType::GetPropertyAt(u32 index)
    {
        if (index >= Properties.GetSize() || IsFunctionType())
            return nullptr;
        return Properties[index];
    }

    u32 StructType::GetFunctionCount()
    {
        return Functions.GetSize();
    }

    FunctionType* StructType::GetFunctionAt(u32 index)
    {
        if (index >= Functions.GetSize())
            return nullptr;
        return Functions[index];
    }
    
} // namespace CE


