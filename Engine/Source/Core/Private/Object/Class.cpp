
#include "CoreMinimal.h"


namespace CE
{

    CE::HashMap<TypeId, StructType*> StructType::registeredStructs{};
    CE::HashMap<Name, StructType*> StructType::registeredStructsByName{};

    bool StructType::IsAssignableTo(TypeId typeId)
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

    const Attribute& StructType::GetAttributes()
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

        return cachedAttributes.IsMap() && cachedAttributes.HasKey(Name(key));
    }

    Attribute StructType::GetAttribute(const CE::String& key)
    {
        if (!attributesCached)
        {
            CacheAllAttributes();
        }

        if (HasAttribute(key))
            return cachedAttributes.GetKeyValue(Name(key));

        return {};
    }

    FieldType* StructType::GetFirstField()
    {
        if (!fieldsCached)
            CacheAllFields();

        if (cachedFields.GetSize() == 0)
            return nullptr;

        return &cachedFields[0];
    }

    Array<FieldType*> StructType::FetchObjectFields()
    {
		Array<FieldType*> result{};

		auto field = GetFirstField();

		while (field != nullptr)
		{
			if (field->IsObjectField())
			{
				result.Add(field);
			}
			field = field->GetNext();
		}

        return result;
    }

    u32 StructType::GetFieldCount()
    {
        if (!fieldsCached)
            CacheAllFields();

        return cachedFields.GetSize();
    }

    FieldType* StructType::GetFieldAt(u32 index)
    {
        if (!fieldsCached)
            CacheAllFields();

        return index < GetFieldCount() ? &cachedFields[index] : nullptr;
    }

    FieldType* StructType::FindFieldWithName(const Name& name)
    {
        if (!fieldsCached)
            CacheAllFields();
        
        if (cachedFieldsMap.KeyExists(name))
            return cachedFieldsMap[name];

        return nullptr;
    }

    FunctionType* StructType::FindFunctionWithName(const Name& name)
    {
        if (!functionsCached)
            CacheAllFunctions();

        if (cachedFunctionsMap.KeyExists(name) && cachedFunctionsMap[name].GetSize() > 0)
            return cachedFunctionsMap[name][0];

        return nullptr;
    }

    CE::Array<FunctionType*> StructType::FindAllFunctionsWithName(const Name& name)
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
        cachedAttributes.isMap = true;
        cachedAttributes.tableValue = {};

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

    void StructType::MergeAttributes(const Attribute& attribs)
    {
        if (!attribs.IsMap())
            return;

        auto& table = attribs.GetTableValue();

        for (const auto& [name, attr] : table)
        {
            if (cachedAttributes.HasKey(name))
            {
                cachedAttributes.GetTableValue()[name] = attr;
            }
            else
            {
                cachedAttributes.GetTableValue().Add({ name, attr });
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
            cachedFields[i].instanceOwner = this;

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
		cachedFunctionsMap.Clear();

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
                    //cachedFunctions.RemoveAll([&](const FunctionType& f) -> bool
                    //    {
                    //        return (func.GetFunctionSignature() == f.GetFunctionSignature()) && func.GetName() == f.GetName();
                    //    });

					cachedFunctions.Add(func);
                }
            }
            else if (type->IsClass())
            {
                auto classType = (ClassType*)type;
                classType->CacheAllFunctions();

                for (const auto& func : classType->cachedFunctions)
                {
					//cachedFunctions.RemoveAll([&](const FunctionType& f) -> bool
					//	{
					//		return (func.GetFunctionSignature() == f.GetFunctionSignature()) && func.GetName() == f.GetName();
					//	});

                    cachedFunctions.Add(func);
                }
            }
        }

		cachedFunctions.AddRange(localFunctions);

		for (int i = 0; i < cachedFunctions.GetSize(); i++)
		{
			cachedFunctions[i].instanceOwner = this;

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

    void StructType::RegisterStructType(StructType* type)
    {
        if (type == nullptr || registeredStructs.KeyExists(type->GetTypeId()))
            return;

        registeredStructs.Add({type->GetTypeId(), type});
        registeredStructsByName.Add({ type->GetTypeName(), type });

        CoreObjectDelegates::onStructRegistered.Broadcast(type);
    }

    void StructType::DeregisterStructType(StructType* type)
    {
        if (type == nullptr || !registeredStructs.KeyExists(type->GetTypeId()))
            return;

        CoreObjectDelegates::onStructDeregistered.Broadcast(type);

        registeredStructs.Remove(type->GetTypeId());
        registeredStructsByName.Remove(type->GetTypeName());
    }

    StructType* StructType::FindStructByName(Name structName)
    {
        if (!structName.IsValid() || !registeredStructsByName.KeyExists(structName))
            return nullptr;

        return registeredStructsByName[structName];
    }

    StructType* StructType::FindStructById(TypeId structTypeId)
    {
        if (structTypeId == 0 || !registeredStructs.KeyExists(structTypeId))
            return nullptr;

        return registeredStructs[structTypeId];
    }

    CE::HashMap<TypeId, ClassType*> ClassType::registeredClasses{};
    CE::HashMap<Name, ClassType*> ClassType::registeredClassesByName{};
    CE::HashMap<TypeId, Array<TypeId>> ClassType::derivedClassesMap{};

	ClassType::ClassType(String name, Internal::IClassTypeImpl* impl, u32 size, String attributes) 
		: StructType(name, nullptr, size, attributes), Impl(impl)
	{
		ConstructInternal();
	}

	void ClassType::ConstructInternal()
	{

	}

	bool ClassType::IsSubclassOf(TypeId classTypeId)
    {
        if (this->GetTypeId() == classTypeId)
            return true;

        if (!superTypesCached)
        {
            CacheSuperTypes();
        }

        for (int i = 0; i < superTypes.GetSize(); i++)
        {
            if (superTypes[i]->IsSubclassOf(classTypeId))
                return true;
        }

        return false;
    }

    bool ClassType::IsObject()
    {
        return IsSubclassOf(TYPEID(Object));
    }

    void ClassType::CacheSuperTypes()
    {
        if (superTypesCached)
            return;

		if (superTypeIds.IsEmpty())
			superTypesCached = true;

        for (int i = 0; i < superTypeIds.GetSize(); i++)
        {
            auto type = GetTypeInfo(superTypeIds[i]);
            if (type == nullptr || !type->IsClass())
                continue;

            superTypes.Add((ClassType*)type);
			superTypesCached = true;
        }

		if (superTypesCached)
			AddDerivedClassToMap(this, this);
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
            if (!derivedClassesMap[parentTypeId].Exists(derivedClass->GetTypeId()))
                derivedClassesMap[parentTypeId].Add(derivedClass->GetTypeId());

            AddDerivedClassToMap(derivedClass, (ClassType*)parentType);
        }
    }

	const Object* ClassType::GetDefaultInstance()
	{
		if (defaultInstance == nullptr)
		{
			auto nameString = GetName().GetLastComponent();

			auto moduleName = GetOwnerModuleName().GetString();
			Package* transientPackage = ModuleManager::Get().GetLoadedModuleTransientPackage(moduleName);
			if (transientPackage == nullptr)
				transientPackage = GetTransientPackage();
			
			defaultInstance = CreateObject<Object>(transientPackage, "CDI_" + nameString, OF_ClassDefaultInstance, this, nullptr);
		}
		return defaultInstance;
	}

    void ClassType::RegisterClassType(ClassType* type)
    {
        if (type == nullptr || registeredClasses.KeyExists(type->GetTypeId()))
            return;
		
        registeredClasses.Add({type->GetTypeId(), type});
        registeredClassesByName.Add({ type->GetTypeName(), type });

        if (!derivedClassesMap.KeyExists(type->GetTypeId()))
        {
            derivedClassesMap.Add({type->GetTypeId(), {}});
        }

        //type->CacheSuperTypes();
        //AddDerivedClassToMap(type, type);

        CoreObjectDelegates::onClassRegistered.Broadcast(type);
    }

    void ClassType::DeregisterClassType(ClassType* type)
    {
        if (type == nullptr || !registeredClasses.KeyExists(type->GetTypeId()))
            return;

        CoreObjectDelegates::onClassDeregistered.Broadcast(type);

        registeredClasses.Remove(type->GetTypeId());
        registeredClassesByName.Remove(type->GetTypeName());
    }

    ClassType* ClassType::FindClassByName(const Name& className)
    {
        if (!className.IsValid() || !registeredClassesByName.KeyExists(className))
            return nullptr;

        return registeredClassesByName[className];
    }

    ClassType* ClassType::FindClassById(TypeId classTypeId)
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
