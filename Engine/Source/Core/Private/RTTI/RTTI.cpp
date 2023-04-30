
#include "CoreMinimal.h"

namespace CE
{
    class Component;

    namespace Internal
    {
    
        CORE_API TypeId GetArrayTypeId()
        {
            return (TypeId)typeid(CE::Array<u8>).hash_code();
        }
    
        CORE_API TypeId GetObjectStoreTypeId()
        {
            return (TypeId)typeid(CE::ObjectStore).hash_code();
        }
    }

    HashMap<Name, TypeInfo*> TypeInfo::registeredTypesByName{};
    HashMap<TypeId, TypeInfo*> TypeInfo::registeredTypeById{};

    Name TypeInfo::currentlyLoadingModule{};
    Name TypeInfo::currentlyUnloadingModule{};

    HashMap<Name, Array<TypeInfo*>> TypeInfo::registeredTypesByModuleName{};

    String Attribute::CleanupAttributeString(const String& inString)
    {
        if (inString.GetLength() == 0)
            return "";

        char* result = new char[inString.GetLength() + 1];
        result[inString.GetLength()] = 0;
        bool isString = false;
        int curParenScope = 0;
        int idx = 0;
        
        for (int i = 0; i < inString.GetLength(); i++)
        {
            if (inString[i] == '"')
            {
                isString = !isString;
                continue;
            }
            
            if (inString[i] == ' ' && !isString)
            {
                continue;
            }

            if (!isString && idx == 0 && inString[i] == '(')
            {
                continue;
            }
            if (!isString && curParenScope == 0 && inString[i] == ')')
            {
                continue;
            }

            if (!isString && inString[i] == '(')
            {
                curParenScope++;
            }
            if (!isString && inString[i] == ')')
            {
                curParenScope--;
            }
            
            result[idx++] = inString[i];
        }

        result[idx++] = 0; // terminating char

        auto str = String(result);
        delete result;
        return str;
    }

    bool Attribute::IsString() const
    {
        return isString;
    }

    bool Attribute::IsMap() const
    {
        return isMap;
    }

    void Attribute::Parse(String attributes, Attribute& outAttrib, bool cleanup)
    {
        outAttrib = Attribute();
        outAttrib.isMap = true;
        outAttrib.tableValue = {};

        int curScope = 0;
        bool isString = false;
        if (cleanup)
            attributes = CleanupAttributeString(attributes);

        int startIdx = 0;

        for (int i = 0; i < attributes.GetLength(); i++)
        {
            if (attributes[i] == '(' || attributes[i] == '[' || attributes[i] == '{')
            {
                curScope++;
                continue;
            }
            else if (attributes[i] == ')' || attributes[i] == ']' || attributes[i] == '}')
            {
                curScope--;
                if (i < attributes.GetLength() - 1)
                    continue;
            }

            bool isLast = i == attributes.GetLength() - 1;

            if ((attributes[i] == ',' && curScope <= 0) || isLast)
            {
                auto attribString = attributes.GetSubstringView(startIdx, i - startIdx + (isLast ? 1 : 0));

                String attribKey = "";
                String attribValue = "";
                bool keyFinished = false;
                int innerScope = 0;

                for (int j = 0; j < attribString.GetSize(); j++)
                {
                    if (attribString[j] == '"')
                    {
                        continue;
                    }
                    if (!keyFinished && attribString[j] == '=')
                    {
                        keyFinished = true;
                        continue;
                    }

                    if (!keyFinished)
                    {
                        attribKey.Append(attribString[j]);
                    }
                    else
                    {
                        attribValue.Append(attribString[j]);
                    }
                }

                if (attribValue.StartsWith("(") && attribValue.EndsWith(")"))
                {
                    Attribute subAttrib;
                    Parse(attribValue.GetSubstringView(1, attribValue.GetLength() - 2), subAttrib, false);
                    outAttrib.tableValue.Add({ attribKey, subAttrib });
                }
                else
                {
                    outAttrib.tableValue.Add({ attribKey, Attribute(attribValue) });
                }

                startIdx = i + 1;
            }
        }
    }

    TypeInfo::TypeInfo(CE::Name name, CE::String attributes)
        : name(name)
    {
        int curScope = 0;
        bool isString = false;

        Attribute::Parse(attributes, this->attributes);
    }

    String TypeInfo::GetDisplayName()
    {
        if (!displayName.IsEmpty())
            return displayName;

        auto tempStr = name.GetString();
        displayName = "";

        for (int i = 0; i < tempStr.GetLength(); i++)
        {
            if (i == 0)
            {
                displayName.Append((char)std::toupper(tempStr[i]));
                continue;
            }
            if (std::isupper(tempStr[i]) && !std::isupper(tempStr[i - 1]))
            {
                displayName.Append(' ');
                displayName.Append(tempStr[i]);
            }
            else
            {
                displayName.Append(tempStr[i]);
            }
        }

        return displayName;
    }

    const Attribute& TypeInfo::GetAttributes()
    {
        return attributes;
    }

    Attribute TypeInfo::GetAttribute(const String& key)
    {
        return attributes.HasKey(key) ? attributes.GetKeyValue(key) : Attribute();
    }

    bool TypeInfo::HasAttribute(const String& key)
    {
        return attributes.HasKey(key);
    }

    bool TypeInfo::IsComponent() const
    {
        return IsClass() && IsAssignableTo(TYPEID(Component));
    }

    Name TypeInfo::GenerateInstanceName(UUID uuid) const
    {
        String typeName = GetName().GetString();
        return String::Format("{0}_{1:x}", typeName, uuid);
    }

    Name TypeInfo::GetOwnerModuleName() const
    {
        return registeredModuleName;
    }

    void TypeInfo::RegisterType(TypeInfo* type)
    {
        if (type == nullptr || registeredTypeById.KeyExists(type->GetTypeId()))
            return;
        
        registeredTypesByName.Add({ type->name, type });
        registeredTypeById.Add({ type->GetTypeId(), type });

        if (currentlyLoadingModule.IsValid())
        {
            if (!registeredTypesByModuleName.KeyExists(currentlyLoadingModule))
                registeredTypesByModuleName.Add({ currentlyLoadingModule, {} });

            registeredTypesByModuleName[currentlyLoadingModule].Add(type);
            type->registeredModuleName = currentlyLoadingModule;
        }

        if (type->IsStruct())
        {
            auto structType = (StructType*)type;
            StructType::RegisterStructType(structType);
        }
        else if (type->IsClass())
        {
            auto classType = (ClassType*)type;
            ClassType::RegisterClassType(classType);
        }
        else if (type->IsEnum())
        {
            auto enumType = (EnumType*)type;
            EnumType::RegisterEnumType(enumType);
        }
    }

    void TypeInfo::DeregisterType(TypeInfo* type)
    {
        if (type == nullptr)
            return;

        if ((!currentlyUnloadingModule.IsValid() && type->registeredModuleName.IsValid()) ||
            (currentlyUnloadingModule.IsValid() && currentlyUnloadingModule == type->registeredModuleName))
        {
            return;
        }

        registeredTypesByName.Remove(type->name);
        registeredTypeById.Remove(type->GetTypeId());

        if (type->registeredModuleName.IsValid() &&
            registeredTypesByModuleName.KeyExists(type->registeredModuleName))
        {
            registeredTypesByModuleName[type->registeredModuleName].Remove(type);
        }

        if (type->IsStruct())
        {
            auto structType = (StructType*)type;
            StructType::DeregisterStructType(structType);
        }
        else if (type->IsClass())
        {
            auto classType = (ClassType*)type;
            ClassType::DeregisterClassType(classType);
        }
        else if (type->IsEnum())
        {
            auto enumType = (EnumType*)type;
            EnumType::DeregisterEnumType(enumType);
        }
    }

    void TypeInfo::DeregisterTypesForModule(ModuleInfo* moduleInfo)
    {
        if (moduleInfo == nullptr || !moduleInfo->moduleName.IsValid())
            return;
        if (!registeredTypesByModuleName.KeyExists(moduleInfo->moduleName))
            return;

        auto& types = registeredTypesByModuleName[moduleInfo->moduleName];

        for (const TypeInfo* typeInfo : types)
        {
            if (typeInfo == nullptr)
                continue;

            registeredTypesByName.Remove(typeInfo->GetName());
            registeredTypeById.Remove(typeInfo->GetTypeId());

            if (typeInfo->IsStruct())
            {
                auto structType = (StructType*)typeInfo;
                StructType::DeregisterStructType(structType);
            }
            else if (typeInfo->IsClass())
            {
                auto classType = (ClassType*)typeInfo;
                ClassType::DeregisterClassType(classType);
            }
            else if (typeInfo->IsEnum())
            {
                auto enumType = (EnumType*)typeInfo;
                EnumType::DeregisterEnumType(enumType);
            }
        }

        types.Clear();
    }

    CORE_API TypeInfo* GetTypeInfo(Name name)
    {
        if (TypeInfo::registeredTypesByName.KeyExists(name))
        {
            return TypeInfo::registeredTypesByName[name];
        }
        return nullptr;
    }

    CORE_API TypeInfo* GetTypeInfo(TypeId typeId)
    {
        if (TypeInfo::registeredTypeById.KeyExists(typeId))
        {
            return TypeInfo::registeredTypeById[typeId];
        }
        return nullptr;
    }

}

// POD Types


CE_RTTI_POD_IMPL(CE, b8)

CE_RTTI_POD_IMPL(CE, s8)
CE_RTTI_POD_IMPL(CE, s16)
CE_RTTI_POD_IMPL(CE, s32)
CE_RTTI_POD_IMPL(CE, s64)

CE_RTTI_POD_IMPL(CE, u8)
CE_RTTI_POD_IMPL(CE, u16)
CE_RTTI_POD_IMPL(CE, u32)
CE_RTTI_POD_IMPL(CE, u64)
CE_RTTI_POD_IMPL(CE, UUID)

CE_RTTI_POD_IMPL(CE, f32)
CE_RTTI_POD_IMPL(CE, f64)

CE_RTTI_POD_IMPL(CE, String)
CE_RTTI_POD_IMPL(CE, Name)
CE_RTTI_POD_IMPL(CE::IO, Path)

CE_RTTI_POD_TEMPLATE_IMPL(CE, Array, CE::u8)


// Math RTTI

CE_RTTI_POD_IMPL(CE, Vec2i)
CE_RTTI_POD_IMPL(CE, Vec3i)
CE_RTTI_POD_IMPL(CE, Vec4i)

CE_RTTI_POD_IMPL(CE, Vec2)
CE_RTTI_POD_IMPL(CE, Vec3)
CE_RTTI_POD_IMPL(CE, Vec4)

CE_RTTI_POD_IMPL(CE, Quat)

CE_RTTI_POD_IMPL(CE, Matrix4x4)

CE_RTTI_POD_IMPL(CE, Color)

