
#include "Object/RTTI.h"

#include "Containers/Array.h"
#include "Object/ObjectStore.h"

namespace CE
{

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

    HashMap<Name, const TypeInfo*> TypeInfo::RegisteredTypes{};
    HashMap<TypeId, const TypeInfo*> TypeInfo::RegisteredTypeIds{};

    String CleanupAttributeString(const String& inString)
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
            }
        }
    }

    void TypeInfo::RegisterType(const TypeInfo* type)
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

    CORE_API const TypeInfo* GetTypeInfo(Name name)
    {
        if (TypeInfo::RegisteredTypes.KeyExists(name))
        {
            return TypeInfo::RegisteredTypes[name];
        }
        return nullptr;
    }

    CORE_API const TypeInfo* GetTypeInfo(TypeId typeId)
    {
        if (TypeInfo::RegisteredTypeIds.KeyExists(typeId))
        {
            return TypeInfo::RegisteredTypeIds[typeId];
        }
        return nullptr;
    }

}

// POD Types



