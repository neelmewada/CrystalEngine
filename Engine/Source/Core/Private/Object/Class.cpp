
#include "Object/Class.h"


namespace CE
{

    CE::HashMap<TypeId, StructType*> StructType::registeredStructs{};
    CE::HashMap<Name, StructType*> StructType::registeredStructsByName{};

    bool StructType::IsAssignableTo(TypeId typeId) const
    {
        if (typeId == this->GetTypeId())
            return true;

        for (auto superTypeId : superTypeIds)
        {
            if (superTypeId == typeId)
            {
                return true;
            }

            auto superType = GetTypeInfo(superTypeId);

            if (superType != nullptr && superType->IsStruct())
            {
                auto structType = (StructType*)superType;
                if (structType->IsAssignableTo(typeId))
                    return true;
            }
            else if (superType != nullptr && superType->IsClass())
            {
                auto classType = (ClassType*)superType;
                if (classType->IsAssignableTo(typeId))
                    return true;
            }
        }

        return false;
    }

    const CE::Array<CE::Attribute>& StructType::GetAttributes()
    {
        if (!attributesCached)
        {
            CacheAllAttributes();
        }

        return cachedAttributes;
    }

    bool StructType::HasAttribute(const String& key)
    {
        if (!attributesCached)
        {
            CacheAllAttributes();
        }

        for (int i = 0; i < cachedAttributes.GetSize(); i++)
        {
            if (cachedAttributes[i].GetKey() == key)
                return true;
        }

        return false;
    }

    String StructType::GetAttributeValue(const CE::String& key)
    {
        if (!attributesCached)
        {
            CacheAllAttributes();
        }

        for (int i = 0; i < cachedAttributes.GetSize(); i++)
        {
            if (cachedAttributes[i].GetKey() == key)
                return cachedAttributes[i].GetValue();
        }

        return "";
    }

    FieldType* StructType::GetFirstField()
    {
        if (!fieldsCached)
            CacheAllFields();

        if (cachedFields.GetSize() == 0)
            return nullptr;

        return &cachedFields[0];
    }

    FieldType* StructType::FindFieldWithName(Name name)
    {
        if (!fieldsCached)
            CacheAllFields();
        
        if (cachedFieldsMap.KeyExists(name))
            return cachedFieldsMap[name];

        return nullptr;
    }

    FunctionType* StructType::FindFunctionWithName(Name name)
    {
        if (!functionsCached)
            CacheAllFunctions();

        if (cachedFunctionsMap.KeyExists(name) && cachedFunctionsMap[name].GetSize() > 0)
            return cachedFunctionsMap[name][0];

        return nullptr;
    }

    CE::Array<FunctionType*> StructType::FindAllFunctionsWithName(Name name)
    {
        if (!functionsCached)
            CacheAllFunctions();
        return cachedFunctionsMap[name];
    }

    void StructType::CacheAllAttributes()
    {
        if (attributesCached)
            return;

        attributesCached = true;
        cachedAttributes.Clear();

        for (int i = 0; i < superTypeIds.GetSize(); i++)
        {
            auto superTypeId = superTypeIds[i];
            auto superType = GetTypeInfo(superTypeId);
            if (superType == nullptr)
                continue;

            if (superType->IsStruct())
            {
                auto superStruct = (StructType*)superType;
                superStruct->CacheAllAttributes();
                MergeAttributes(superStruct->cachedAttributes);
            }
            else if (superType->IsClass())
            {
                auto superClass = (ClassType*)superType;
                superClass->CacheAllAttributes();
                MergeAttributes(superClass->cachedAttributes);
            }
        }

        MergeAttributes(attributes);
    }

    void StructType::MergeAttributes(const CE::Array<Attribute>& attribs)
    {
        for (int i = 0; i < attribs.GetSize(); i++)
        {
            const auto& attribToAdd = attribs[i];
            auto index = cachedAttributes.IndexOf([attribToAdd](const Attribute& a) -> bool
                    {
                        return a.GetKey() == attribToAdd.GetKey();
                    });
            if (index >= 0)
            {
                cachedAttributes[index].value = attribToAdd.value;
            }
            else
            {
                cachedAttributes.Add(attribToAdd);
            }
        }
    }

    void StructType::CacheAllFields()
    {
        if (fieldsCached)
            return;

        fieldsCached = true;
        cachedFields.Clear();

        for (int i = 0; i < superTypeIds.GetSize(); i++)
        {
            auto typeId = superTypeIds[i];
            if (typeId == this->GetTypeId())
                continue;

            const TypeInfo* type = GetTypeInfo(typeId);
            if (type->IsStruct())
            {
                auto structType = (StructType*)type;
                structType->CacheAllFields();
                cachedFields.AddRange(structType->cachedFields);
            }
            else if (type->IsClass())
            {
                auto classType = (ClassType*)type;
                classType->CacheAllFields();
                cachedFields.AddRange(classType->cachedFields);
            }
        }

        cachedFields.AddRange(localFields);

        for (int i = 0; i < cachedFields.GetSize(); i++)
        {
            cachedFields[i].owner = this;

            if (i == cachedFields.GetSize() - 1)
            {
                cachedFields[i].next = nullptr;
            }
            else
            {
                cachedFields[i].next = &cachedFields[i + 1];
            }
            
            cachedFieldsMap.Add({ cachedFields[i].GetName(), &cachedFields[i] });
        }
    }

    void StructType::CacheAllFunctions()
    {
        if (functionsCached)
            return;

        functionsCached = true;
        cachedFunctions.Clear();

        for (int i = 0; i < superTypeIds.GetSize(); i++)
        {
            auto typeId = superTypeIds[i];
            if (typeId == this->GetTypeId())
                continue;

            const TypeInfo* type = GetTypeInfo(typeId);
            if (type->IsStruct())
            {
                auto structType = (StructType*)type;
                structType->CacheAllFunctions();

                for (const auto& func : structType->cachedFunctions)
                {
                    if (!cachedFunctions.Exists([&](const FunctionType& f) -> bool
                        {
                            return (func.GetFunctionSignature() == f.GetFunctionSignature()) && func.GetName() == f.GetName();
                        }))
                    {
                        cachedFunctions.Add(func);
                    }
                }
            }
            else if (type->IsClass())
            {
                auto classType = (ClassType*)type;
                classType->CacheAllFunctions();

                for (const auto& func : classType->cachedFunctions)
                {
                    if (!cachedFunctions.Exists([&](const FunctionType& f) -> bool
                        {
                            return (func.GetFunctionSignature() == f.GetFunctionSignature()) && func.GetName() == f.GetName();
                        }))
                    {
                        cachedFunctions.Add(func);
                    }
                }
            }

            cachedFunctions.AddRange(localFunctions);

            for (int i = 0; i < cachedFunctions.GetSize(); i++)
            {
                cachedFunctions[i].owner = this;

                if (i == cachedFunctions.GetSize() - 1)
                {
                    cachedFunctions[i].next = nullptr;
                }
                else
                {
                    cachedFunctions[i].next = &cachedFunctions[i + 1];
                }

                if (cachedFunctionsMap.KeyExists(cachedFunctions[i].GetName()))
                    cachedFunctionsMap[cachedFunctions[i].GetName()].Add(&cachedFunctions[i]);
                else
                    cachedFunctionsMap.Add({ cachedFunctions[i].GetName(), { &cachedFunctions[i] } });
            }
        }
    }

    void StructType::RegisterStruct(StructType* type)
    {
        if (type == nullptr || registeredStructs.KeyExists(type->GetTypeId()))
            return;

        registeredStructs.Add({type->GetTypeId(), type});
    }

    void StructType::DeregisterStruct(StructType* type)
    {
        if (type == nullptr || !registeredStructs.KeyExists(type->GetTypeId()))
            return;

        registeredStructs.Remove(type->GetTypeId());
    }

    StructType* StructType::FindStructByName(Name structName)
    {
        if (!structName.IsValid() || !registeredStructsByName.KeyExists(structName))
            return nullptr;

        return registeredStructsByName[structName];
    }

    StructType* StructType::FindStructByTypeId(TypeId structTypeId)
    {
        if (structTypeId == 0 || !registeredStructs.KeyExists(structTypeId))
            return nullptr;

        return registeredStructs[structTypeId];
    }

    CE::HashMap<TypeId, ClassType*> ClassType::registeredClasses{};
    CE::HashMap<Name, ClassType*> ClassType::registeredClassesByName{};
    CE::HashMap<TypeId, Array<TypeId>> ClassType::derivedClassesMap{};

    bool ClassType::IsObject()
    {
        if (this->GetTypeId() == TYPEID(CE::Object))
            return true;

        if (!superTypesCached)
        {
            CacheSuperTypes();
        }

        for (int i = 0; i < superTypes.GetSize(); i++)
        {
            if (superTypes[i]->IsObject())
                return true;
        }

        return false;
    }

    void ClassType::CacheSuperTypes()
    {
        if (superTypesCached)
            return;

        superTypesCached = true;

        for (int i = 0; i < superTypeIds.GetSize(); i++)
        {
            auto type = GetTypeInfo(superTypeIds[i]);
            if (type == nullptr || !type->IsClass())
                continue;

            superTypes.Add((ClassType*)type);
        }
    }

    void ClassType::AddDerivedClassToMap(ClassType* derivedClass, ClassType* parentSearchPath)
    {
        if (derivedClass == nullptr || parentSearchPath == nullptr)
            return;

        for (int i = 0; i < parentSearchPath->GetSuperTypesCount(); i++)
        {
            auto parentTypeId = parentSearchPath->GetSuperType(i);
            auto parentType = GetTypeInfo(parentTypeId);
            if (parentType == nullptr || !parentType->IsClass())
                continue;

            if (!derivedClassesMap.KeyExists(parentTypeId))
                derivedClassesMap.Add({ parentTypeId, {} });
            derivedClassesMap[parentTypeId].Add(derivedClass->GetTypeId());

            AddDerivedClassToMap(derivedClass, (ClassType*)parentType);
        }
    }

    void ClassType::RegisterClass(ClassType* type)
    {
        if (type == nullptr || registeredClasses.KeyExists(type->GetTypeId()))
            return;

        registeredClasses.Add({type->GetTypeId(), type});
        registeredClassesByName.Add({ type->GetName(), type });

        if (!derivedClassesMap.KeyExists(type->GetTypeId()))
        {
            derivedClassesMap.Add({type->GetTypeId(), {}});
        }

        type->CacheSuperTypes();
        AddDerivedClassToMap(type, type);
    }

    void ClassType::DeregisterClass(ClassType* type)
    {
        if (type == nullptr || !registeredClasses.KeyExists(type->GetTypeId()))
            return;

        registeredClasses.Remove(type->GetTypeId());
        registeredClassesByName.Remove(type->GetName());

        type->CacheSuperTypes();
    }

    ClassType* ClassType::FindClassByName(Name className)
    {
        if (!className.IsValid() || !registeredClassesByName.KeyExists(className))
            return nullptr;

        return registeredClassesByName[className];
    }

    ClassType* ClassType::FindClassByTypeId(TypeId classTypeId)
    {
        if (classTypeId == 0 || !registeredClasses.KeyExists(classTypeId))
            return nullptr;

        return registeredClasses[classTypeId];
    }

    Array<TypeId> ClassType::GetDerivedClassesTypeId() const
    {
        if (!derivedClassesMap.KeyExists(GetTypeId()))
            return {};

        return derivedClassesMap[GetTypeId()];
    }

    Array<ClassType*> ClassType::GetDerivedClasses() const
    {
        auto typeIdArray = GetDerivedClassesTypeId();

        Array<ClassType*> result{};

        for (int i = 0; i < typeIdArray.GetSize(); i++)
        {
            auto type = GetTypeInfo(typeIdArray[i]);
            if (type == nullptr || !type->IsClass())
                continue;

            result.Add((ClassType*)type);
        }

        return result;
    }

} // namespace CE
