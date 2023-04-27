
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

    HashMap<Name, TypeInfo*> TypeInfo::RegisteredTypes{};
    HashMap<TypeId, TypeInfo*> TypeInfo::RegisteredTypeIds{};

    static String CleanupAttributeString(const String& inString)
    {
        char* result = new char[inString.GetLength() + 1];
        result[inString.GetLength()] = 0;
        bool isString = false;
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
            
            result[idx++] = inString[i];
        }

        result[idx++] = 0;
        
        return String(result);
    }

    void Attribute::Parse(String attributes, CE::Array<Attribute>& outResult)
    {
        outResult.Clear();
        int curScope = 0;
        bool isString = false;
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

                outResult.Add(Attribute(attribKey, attribValue));

                startIdx = i + 1;
            }
        }
    }

    TypeInfo::TypeInfo(CE::Name name, CE::String attributes)
        : name(name)
    {
        int curScope = 0;
        bool isString = false;
        String attribs = CleanupAttributeString(attributes);
        this->attributes.Clear();
        
        int startIdx = 0;

        for (int i = 0; i < attribs.GetLength(); i++)
        {
            if (attribs[i] == '(' || attribs[i] == '[' || attribs[i] == '{')
            {
                curScope++;
                continue;
            }
            else if (attribs[i] == ')' || attribs[i] == ']' || attribs[i] == '}')
            {
                curScope--;
                continue;
            }

            bool isLast = i == attribs.GetLength() - 1;

            if ((attribs[i] == ',' && curScope <= 0) || isLast)
            {
                auto attribString = attribs.GetSubstringView(startIdx, i - startIdx + (isLast ? 1 : 0));
                
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

                this->attributes.Add(Attribute(attribKey, attribValue));

                startIdx = i + 1;
            }
        }
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

    const CE::Array<CE::Attribute>& TypeInfo::GetAttributes()
    {
        return GetLocalAttributes();
    }

    String TypeInfo::GetLocalAttributeValue(const String& key) const
    {
        for (int i = 0; i < attributes.GetSize(); i++)
        {
            if (attributes[i].GetKey() == key)
            {
                return attributes[i].GetValue();
            }
        }

        return "";
    }

    bool TypeInfo::HasLocalAttribute(const String& key) const
    {
        for (int i = 0; i < attributes.GetSize(); i++)
        {
            if (attributes[i].GetKey() == key)
            {
                return true;
            }
        }

        return false;
    }

    String TypeInfo::GetAttributeValue(const String& key)
    {
        return GetLocalAttributeValue(key);
    }

    bool TypeInfo::HasAttribute(const String& key)
    {
        return HasLocalAttribute(key);
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

    void TypeInfo::RegisterType(TypeInfo* type)
    {
        if (type == nullptr)
            return;
        
        if (!RegisteredTypes.KeyExists(type->name))
        {
            RegisteredTypes.Add({ type->name, type });
        }
        if (!RegisteredTypeIds.KeyExists(type->GetTypeId()))
        {
            RegisteredTypeIds.Add({ type->GetTypeId(), type });
        }
    }

    void TypeInfo::DeregisterType(TypeInfo* type)
    {
        if (type == nullptr)
            return;

        RegisteredTypes.Remove(type->name);
        RegisteredTypeIds.Remove(type->GetTypeId());
    }

    CORE_API TypeInfo* GetTypeInfo(Name name)
    {
        if (TypeInfo::RegisteredTypes.KeyExists(name))
        {
            return TypeInfo::RegisteredTypes[name];
        }
        return nullptr;
    }

    CORE_API TypeInfo* GetTypeInfo(TypeId typeId)
    {
        if (TypeInfo::RegisteredTypeIds.KeyExists(typeId))
        {
            return TypeInfo::RegisteredTypeIds[typeId];
        }
        return nullptr;
    }

}

// POD Types



