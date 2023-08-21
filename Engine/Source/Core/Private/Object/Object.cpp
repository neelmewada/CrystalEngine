
#include "CoreMinimal.h"

namespace CE
{
	static RecursiveMutex globalMutex{};

    Object::Object() : name("Object"), uuid(UUID())
    {
        ConstructInternal();
    }

    Object::Object(const ObjectInitializer& initializer)
    {
        ConstructInternal(const_cast<ObjectInitializer*>(&initializer));
    }

	Object::~Object()
	{
		// Never call delete directly. Use RequestDestroy() instead
	}

	void Object::RequestDestroy()
	{
		if (this == nullptr)
			return;
		
		OnBeforeDestroy();

		// Unbind signals
		UnbindAllSignals(this);

		auto package = GetPackage();
		if (package != nullptr)
		{
			package->OnObjectUnloaded(this); // Mark the object as 'unloaded'
		}

		// Detach this object from outer
		if (outer != nullptr)
		{
			outer->DetachSubobject(this);
		}
		outer = nullptr;

		// Delete all attached subobjects
		if (attachedObjects.GetObjectCount() > 0)
		{
			for (auto [_, subobject] : attachedObjects)
			{
				subobject->outer = nullptr; // set subobject's outer to null, so it cannot call Detach on destruction
				subobject->RequestDestroy();
			}
		}
		attachedObjects.RemoveAll();

		delete this;
	}

	bool Object::IsOfType(ClassType* classType)
	{
		if (classType == nullptr)
			return false;

		return GetClass()->IsSubclassOf(classType);
	}

    void Object::ConstructInternal()
    {
        auto initializer = ObjectThreadContext::Get().TopInitializer();
        ASSERT(initializer != nullptr, "An object was contructed without any initializers set! This usually happens when you construct an object using 'new' operator.");
        ObjectThreadContext::Get().PopInitializer();
        ConstructInternal(initializer);
    }

    void Object::ConstructInternal(ObjectInitializer* initializer)
    {
        ASSERT(initializer != nullptr, "An object was contructed without any initializers set! This usually happens when you construct an object using 'new' operator.");
        ASSERT(initializer->objectClass != nullptr, "Object initializer passed with null objectClass!");

        this->objectFlags = initializer->GetObjectFlags();
        if (initializer->uuid != 0)
            this->uuid = initializer->uuid;
        this->name = initializer->name;
    }

    void Object::AttachSubobject(Object* subobject)
    {
        if (subobject == nullptr || attachedObjects.ObjectExists(subobject->GetUuid()))
            return;
        attachedObjects.AddObject(subobject);
        subobject->outer = this;

		auto package = GetPackage();
		if (package != nullptr)
		{
			package->loadedObjects[subobject->GetUuid()] = subobject;
		}

		OnSubobjectAttached(subobject);
    }

    void Object::DetachSubobject(Object* subobject)
    {
        if (subobject == nullptr || !attachedObjects.ObjectExists(subobject->GetUuid()))
            return;

        subobject->outer = nullptr;
        attachedObjects.RemoveObject(subobject);

		auto package = GetPackage();
		if (package != nullptr)
		{
			package->loadedObjects.Remove(subobject->GetUuid());
		}

		OnSubobjectDetached(subobject);
    }

	bool Object::HasSubobject(Object* subobject)
	{
		if (subobject == nullptr)
			return false;
		if (attachedObjects.ObjectExists(subobject->GetUuid()))
		{
			return true;
		}
		return false;
	}

    bool Object::ObjectPresentInHierarchy(Object* searchObject)
    {
		if (searchObject == nullptr)
			return false;
		if (searchObject == this)
			return true;

		if (HasSubobject(searchObject))
		{
			return true;
		}

		for (auto [_, subobject] : attachedObjects)
		{
			if (subobject->HasSubobject(searchObject))
				return true;
		}

        return false;
    }

	bool Object::ParentExistsInChain(Object* parent) const
	{
		Object* outer = GetOuter();

		while (outer != nullptr)
		{
			if (outer == parent)
				return true;
			outer = outer->GetOuter();
		}

		return false;
	}

	bool Object::IsTransient()
	{
		Package* package = GetPackage();
		if (package != nullptr && package != this && package->IsTransient())
			return true;
		return HasAnyObjectFlags(OF_Transient);
	}

    Package* Object::GetPackage()
    {
		if (GetClass()->IsSubclassOf<Package>())
			return (Package*)this;

        auto outerObject = outer;
        if (outerObject == nullptr)
            return nullptr;
        
        while (outerObject != nullptr)
        {
            if (outerObject->GetClass()->IsSubclassOf<Package>())
                return (Package*)outerObject;
            outerObject = outerObject->outer;
        }
        
        return nullptr;
    }

	Name Object::GetPathInPackage()
	{
        if (IsPackage())
            return "";
        
		String path = name.GetString();
		
		auto outerObject = outer;

		while (outerObject != nullptr && !outerObject->IsPackage())
		{
			auto outerPathName = outerObject->GetName();
			if (outerPathName.IsValid())
				path = outerPathName.GetString() + "." + path;
			outerObject = outerObject->outer;
		}

		return path;
	}

	void Object::FetchObjectReferences(HashMap<UUID, Object*>& outReferences)
	{
		for (const auto& [uuid, subobject] : attachedObjects)
		{
			if (subobject == nullptr || uuid == 0)
				continue;

			if (!outReferences.KeyExists(uuid))
				outReferences[uuid] = subobject;
			subobject->FetchObjectReferences(outReferences);
		}

		auto objectFields = GetClass()->FetchObjectFields();

		for (auto field : objectFields)
		{
			auto object = field->GetFieldValue<Object*>(this);
			if (object != nullptr)
			{
				if (!outReferences.KeyExists(object->GetUuid()))
					outReferences[object->GetUuid()] = object;
			}
		}
	}

    void Object::LoadConfig(ClassType* configClass, String fileName)
    {
        if (configClass == NULL)
            configClass = GetClass();
		if (!GetClass()->IsSubclassOf(configClass)) // `this` class should always be same as or a subclass of `configClass`
			return;
        
        //if (fileName.IsEmpty() && configClass->HasAttribute("Config"))
        //{
        //    fileName = configClass->GetAttribute("Config").GetStringValue();
        //}
        //if (fileName.IsEmpty())
        //    return;
        
        auto config = gConfigCache->GetConfigFile(ConfigType(fileName));
        auto classTypeName = configClass->GetTypeName();
        
        if (config == nullptr)
            return;
        if (!config->SectionExists(classTypeName))
            return;

        ConfigSection& configSection = config->Get(classTypeName);
        FieldType* configField = configClass->GetFirstField();
        config->SectionExists("");
        
        while (configField != nullptr)
        {
            if (!configField->HasAnyFieldFlags(FIELD_Config))
            {
				configField = configField->GetNext();
                continue;
            }

			auto field = GetClass()->FindFieldWithName(configField->GetName());
			if (field->GetTypeId() != configField->GetTypeId())
			{
				configField = configField->GetNext();
				continue;
			}
            
            auto fieldTypeId = field->GetDeclarationTypeId();
            auto fieldName = field->GetName().GetString();
			auto fieldDeclType = field->GetDeclarationType();

            if (!configSection.KeyExists(fieldName) || !configSection[fieldName].IsValid())
            {
				configField = configField->GetNext();
                continue;
            }
            
            ConfigValue& configValue = configSection[fieldName];
            String stringValue = configSection[fieldName].GetString();
			if (stringValue.StartsWith("\""))
				stringValue = stringValue.GetSubstringView(1);
			if (stringValue.EndsWith("\""))
				stringValue = stringValue.GetSubstringView(0, stringValue.GetLength() - 1);
            
            if (fieldTypeId == TYPEID(String))
            {
                field->SetFieldValue<String>(this, stringValue);
            }
            else if (fieldTypeId == TYPEID(IO::Path))
            {
                field->SetFieldValue<IO::Path>(this, IO::Path(stringValue));
            }
            else if (fieldTypeId == TYPEID(Name))
            {
                field->SetFieldValue<Name>(this, Name(stringValue));
            }
            else if (fieldTypeId == TYPEID(UUID))
            {
                u64 uuid = 0;
                if (String::TryParse(stringValue, uuid))
                {
                    field->SetFieldValue<UUID>(this, UUID(uuid));
                }
            }
            else if (field->IsIntegerField())
            {
                s64 value = 0;
                if (String::TryParse(stringValue, value))
                {
                    if (fieldTypeId == TYPEID(u8))
                        field->SetFieldValue<u8>(this, (u8)value);
                    else if (fieldTypeId == TYPEID(u16))
                        field->SetFieldValue<u16>(this, (u16)value);
                    else if (fieldTypeId == TYPEID(u32))
                        field->SetFieldValue<u32>(this, (u32)value);
                    else if (fieldTypeId == TYPEID(u64))
                        field->SetFieldValue<u64>(this, (u64)value);
                    else if (fieldTypeId == TYPEID(s8))
                        field->SetFieldValue<s8>(this, (s8)value);
                    else if (fieldTypeId == TYPEID(s16))
                        field->SetFieldValue<s16>(this, (s16)value);
                    else if (fieldTypeId == TYPEID(s32))
                        field->SetFieldValue<s32>(this, (s32)value);
                    else if (fieldTypeId == TYPEID(s64))
                        field->SetFieldValue<s64>(this, (s64)value);
                }
            }
            else if (field->IsDecimalField())
            {
                f32 value = 0;
                if (String::TryParse(stringValue, value))
                {
                    if (fieldTypeId == TYPEID(f32))
                        field->SetFieldValue<f32>(this, value);
                    else if (fieldTypeId == TYPEID(f64))
                        field->SetFieldValue<f64>(this, value);
                }
            }
            else if (fieldTypeId == TYPEID(bool))
            {
                bool value = false;
                if (String::TryParse(stringValue, value))
                {
                    field->SetFieldValue<bool>(this, value);
                }
            }
			else if (fieldDeclType != nullptr && fieldDeclType->IsStruct())
			{
				StructType* structType = (StructType*)fieldDeclType;
				void* structInstance = field->GetFieldInstance(this);

				const String& string = configValue.GetString();

				if (string.StartsWith("(") && string.EndsWith(")"))
				{
					ConfigParseStruct(string.GetSubstringView(1, string.GetLength() - 2), structInstance, structType);
				}
			}
            else if (field->IsArrayType() && configValue.IsValid())
            {
                Array<String>& array = configValue.GetArray();
                if (array.IsEmpty())
                {
                    auto& arrayValue = field->GetFieldValue<Array<u8>>(this);
                    arrayValue.Clear();
                }
                else
                {
                    Array<u8>& arrayValue = field->GetFieldValue<Array<u8>>(this);
                    bool isMap = false;
                    isMap = array[0].StartsWith("(");
                    
                    TypeInfo* underlyingType = field->GetUnderlyingType();
                    field->ResizeArray(this, array.GetSize());
                    int idx = -1;
                    
                    for (const String& value : array)
                    {
                        idx++;
                        
                        if (isMap)
                        {
                            if (underlyingType != nullptr && underlyingType->IsStruct())
                            {
                                StructType* structType = (StructType*)underlyingType;
                                void* structInstance = &arrayValue[0] + idx * structType->GetSize();
                                
                                ConfigParseStruct(value.GetSubstringView(1, value.GetLength() - 2), structInstance, structType);
                            }
                        }
                        else
                        {
							Array<FieldType> arrayFieldList = field->GetArrayFieldList(this);

							void* arrayInstance = arrayValue.Begin();

							for (int i = 0; i < arrayFieldList.GetSize(); i++)
							{
								FieldType* field = &arrayFieldList[i];
								ConfigParsePOD(array[i], arrayInstance, field);
							}
                        }
                    }
                }
            }
            
			configField = configField->GetNext();
        }
    }

	Object* Object::CreateDefaultSubobject(ClassType* classType, const String& name, ObjectFlags flags)
	{
		return CreateObject<Object>(this, name, flags, classType);
	}

	void Object::LoadFromTemplate(Object* templateObject)
	{
		if (templateObject == nullptr)
			return;

		auto templateClass = templateObject->GetClass();
		auto thisClass = this->GetClass();

		if (!thisClass->IsSubclassOf(templateClass))
			return;

		for (auto field = templateClass->GetFirstField(); field != nullptr; field = field->GetNext())
		{
			auto destField = thisClass->FindFieldWithName(field->GetName());
			if (destField->GetTypeId() != field->GetTypeId()) // Type mismatch
				continue;
			if (destField->IsReadOnly())
				continue;

			if (field->IsObjectField()) // Deep copy object fields
			{
				Object* objectToCopy = field->GetFieldValue<Object*>(templateObject);
				if (objectToCopy == nullptr)
				{
					destField->SetFieldValue<Object*>(this, nullptr);
				}
				else
				{
					if (objectToCopy->GetOuter() == templateObject)
					{
						// Deep copy sub-object
						Object* deepCopy = CreateObject<Object>(this, objectToCopy->GetName().GetString(), objectToCopy->GetFlags(), objectToCopy->GetClass(), objectToCopy);
						destField->SetFieldValue<Object*>(this, deepCopy);
					}
					else
					{
						// Shallow copy external object references
						destField->SetFieldValue<Object*>(this, objectToCopy);
					}
				}
			}
			else
			{
				field->CopyTo(templateObject, destField, this);
			}
		}
	}

	void Object::LoadDefaults()
	{
		// Load Base.ini into Object class
		LoadConfig(GetStaticClass<Object>(), "");

		Array<ClassType*> superChain{};
		
		ClassType* super = GetClass();

		while (super != nullptr)
		{
			superChain.Add(super);
			if (super->GetSuperClassCount() > 0)
				super = super->GetSuperClass(0);
			else
				super = nullptr;
		}

		for (int i = superChain.GetSize() - 1; i >= 0; i--)
		{
			if (superChain[i]->GetTypeId() == TYPEID(Object))
				continue;

			auto fileName = superChain[i]->GetAttribute("Config").GetStringValue();
			LoadConfig(superChain[i], fileName);
		}
	}

	void Object::ConfigParseStruct(const String& value, void* instance, StructType* structType)
    {
        Array<String> split = value.Split(',');
        for (const String& item : split)
        {
            Array<String> valueSplit = item.Split('=');
            if (valueSplit.GetSize() >= 2)
            {
                String& lhs = valueSplit[0];
                String& rhs = valueSplit[1];

				if (rhs.StartsWith("\""))
					rhs = rhs.GetSubstringView(1);
				if (rhs.EndsWith("\""))
					rhs = rhs.GetSubstringView(0, rhs.GetLength() - 1);
                
                FieldType* field = structType->FindFieldWithName(lhs);
                if (field == nullptr)
                    continue;
                
                if (field->GetDeclarationType()->IsPOD())
                {
                    ConfigParsePOD(rhs, instance, field);
                }
                
            }
        }
    }

    void Object::ConfigParsePOD(const String& value, void* instance, FieldType* field)
    {
        if (field == nullptr || instance == nullptr)
            return;
        
        TypeId fieldTypeId = field->GetTypeId();
		TypeInfo* fieldDeclType = field->GetDeclarationType();
        
        if (field->IsStringField())
        {
            field->SetFieldValue(instance, value);
        }
        else if (field->GetTypeId() == TYPEID(Name))
        {
            field->SetFieldValue(instance, Name(value));
        }
        else if (field->GetTypeId() == TYPEID(UUID))
        {
            u64 intValue = 0;
            if (String::TryParse(value, intValue))
            {
                field->SetFieldValue(instance, UUID(intValue));
            }
        }
        else if (field->IsIntegerField())
        {
            s64 intValue = 0;
            if (String::TryParse(value, intValue))
            {
                if (fieldTypeId == TYPEID(u8))
                    field->SetFieldValue<u8>(instance, (u8)intValue);
                else if (fieldTypeId == TYPEID(u16))
                    field->SetFieldValue<u16>(instance, (u16)intValue);
                else if (fieldTypeId == TYPEID(u32))
                    field->SetFieldValue<u32>(instance, (u32)intValue);
                else if (fieldTypeId == TYPEID(u64))
                    field->SetFieldValue<u64>(instance, (u64)intValue);
                else if (fieldTypeId == TYPEID(s8))
                    field->SetFieldValue<s8>(instance, (s8)intValue);
                else if (fieldTypeId == TYPEID(s16))
                    field->SetFieldValue<s16>(instance, (s16)intValue);
                else if (fieldTypeId == TYPEID(s32))
                    field->SetFieldValue<s32>(instance, (s32)intValue);
                else if (fieldTypeId == TYPEID(s64))
                    field->SetFieldValue<s64>(instance, (s64)intValue);
            }
        }
        else if (field->IsDecimalField())
        {
            f32 decimalValue = 0;
            if (String::TryParse(value, decimalValue))
            {
                if (fieldTypeId == TYPEID(f32))
                    field->SetFieldValue<f32>(instance, decimalValue);
                else if (fieldTypeId == TYPEID(f64))
                    field->SetFieldValue<f64>(instance, decimalValue);
            }
        }
		else if (field->GetTypeId() == TYPEID(bool))
		{
			bool boolValue = false;
			if (String::TryParse(value, boolValue))
			{
				field->SetFieldValue<bool>(instance, boolValue);
			}
		}
		else if (fieldDeclType != nullptr && fieldDeclType->IsStruct())
		{
			StructType* structType = (StructType*)fieldDeclType;
			void* structInstance = field->GetFieldInstance(instance);

			if (value.StartsWith("(") && value.EndsWith(")"))
				ConfigParseStruct(value.GetSubstringView(1, value.GetLength() - 2), structInstance, structType);
		}
    }

	// - Bindings API -

	HashMap<void*, Array<SignalBinding>> Object::outgoingBindingsMap{};
	HashMap<void*, Array<SignalBinding>> Object::incomingBindingsMap{};

	void Object::EmitSignal(const String& name, const Array<Variant>& args)
	{
		Object::EmitSignal(this, name, args);
	}

	DelegateHandle Object::BindInternal(void* sourceInstance, FunctionType* sourceFunction, Delegate<void(const Array<Variant>&)> delegate)
	{
		if (sourceInstance == nullptr || sourceFunction == nullptr || !delegate.IsValid())
			return 0;

		LockGuard<RecursiveMutex> lock{ globalMutex };
		
		auto& outgoingBindings = outgoingBindingsMap[sourceInstance];

		outgoingBindings.Add({});
		auto& binding = outgoingBindings.Top();

		binding.signalInstance = sourceInstance;
		binding.signalFunction = sourceFunction;
		binding.boundInstance = nullptr;
		binding.boundFunction = nullptr;

		binding.boundDelegate = delegate;

		return delegate.GetHandle();
	}

	void Object::EmitSignal(void* signalInstance, const String& name, const Array<Variant>& args)
	{
		LockGuard<RecursiveMutex> lock{ globalMutex };

		Array<TypeId> argHashes{};
		for (const Variant& arg : args)
		{
			argHashes.Add(arg.GetValueTypeId());
		}
		TypeId signature = (TypeId)GetCombinedHashes(argHashes);

		const Array<SignalBinding>& bindings = outgoingBindingsMap[signalInstance];

		for (const auto& binding : bindings)
		{
			if (binding.signalFunction == nullptr ||
				binding.signalInstance == nullptr)
				continue;

			if (binding.signalFunction->IsSignalFunction() &&
				binding.signalFunction->GetName() == name &&
				binding.boundDelegate.IsValid())
			{
				binding.boundDelegate.Invoke(args);
			}

			if (binding.boundFunction == nullptr ||
				binding.boundInstance == nullptr)
				continue;

			if (binding.signalFunction->IsSignalFunction() &&
				binding.signalFunction->GetName() == name &&
				binding.signalFunction->GetFunctionSignature() == signature &&
				binding.boundFunction->GetFunctionSignature() == signature)
			{
				binding.boundFunction->Invoke(binding.boundInstance, args);
			}
		}
	}

	bool Object::Bind(void* sourceInstance, FunctionType* sourceFunction, void* destinationInstance, FunctionType* destinationFunction)
	{
		if (sourceInstance == nullptr || sourceFunction == nullptr || destinationInstance == nullptr || destinationFunction == nullptr)
			return false;

		if (sourceFunction->GetFunctionSignature() != destinationFunction->GetFunctionSignature())
			return false;

		LockGuard<RecursiveMutex> lock{ globalMutex };

		auto& outgoingBindings = outgoingBindingsMap[sourceInstance];
		auto& incomingBindings = incomingBindingsMap[destinationInstance];

		SignalBinding binding{};
		binding.signalInstance = sourceInstance;
		binding.signalFunction = sourceFunction;
		binding.boundInstance = destinationInstance;
		binding.boundFunction = destinationFunction;

		outgoingBindings.Add(binding);
		incomingBindings.Add(binding);

		return true;
	}

	void Object::UnbindSignals(void* toInstance, void* fromInstance)
	{
		LockGuard<RecursiveMutex> lock{ globalMutex };

		auto& incoming = incomingBindingsMap[toInstance];

		for (int i = incoming.GetSize() - 1; i >= 0; i--)
		{
			if (incoming[i].signalInstance == fromInstance)
				incoming.RemoveAt(i);
		}
	}

	void Object::UnbindAllIncomingSignals(void* toInstance)
	{
		LockGuard<RecursiveMutex> lock{ globalMutex };

		auto& incoming = incomingBindingsMap[toInstance];

		for (int i = incoming.GetSize() - 1; i >= 0; i--)
		{
			incoming.RemoveAt(i);
		}
	}

	void Object::UnbindAllOutgoingSignals(void* fromInstance)
	{
		LockGuard<RecursiveMutex> lock{ globalMutex };

		auto& outgoing = outgoingBindingsMap[fromInstance];

		for (int i = outgoing.GetSize() - 1; i >= 0; i--)
		{
			outgoing.RemoveAt(i);
		}
	}

	void Object::UnbindAllSignals(void* instance)
	{
		LockGuard<RecursiveMutex> lock{ globalMutex };

		if (outgoingBindingsMap.KeyExists(instance))
		{
			auto& outgoing = outgoingBindingsMap[instance];
			for (int i = outgoing.GetSize() - 1; i >= 0; i--) // OUTGOING
			{
				auto& binding = outgoing[i];

				if (binding.boundInstance != nullptr)
				{
					UnbindSignals(binding.boundInstance, instance);
				}
			}

			outgoing.Clear();
			outgoingBindingsMap.Remove(instance);
		}

		if (incomingBindingsMap.KeyExists(instance))
		{
			auto& incoming = incomingBindingsMap[instance];
			for (int i = incoming.GetSize() - 1; i >= 0; i--) // INCOMING
			{
				auto& binding = incoming[i];

				auto& outgoingFrom = outgoingBindingsMap[binding.signalInstance];

				for (int j = outgoingFrom.GetSize() - 1; j >= 0; j--)
				{
					if (outgoingFrom[j].boundInstance == instance)
						outgoingFrom.RemoveAt(j);
				}
			}

			incoming.Clear();
			incomingBindingsMap.Remove(instance);
		}
	}

    void Object::Unbind(void* instance, DelegateHandle delegateInstance)
    {
		LockGuard<RecursiveMutex> lock{ globalMutex };

        auto& outgoing = outgoingBindingsMap[instance];
        for (int i = outgoing.GetSize() - 1; i >= 0; i--) // OUTGOING
        {
            auto& binding = outgoing[i];

            if (binding.boundDelegate.IsValid() && binding.boundDelegate.GetHandle() == delegateInstance)
            {
                outgoing.RemoveAt(i);
            }
        }
    }

}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Object)

