
#include "CoreMinimal.h"

namespace CE
{
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
        // Detach this object from outer
        if (outer != nullptr)
        {
            outer->DetachSubobject(this);
        }
        
        // Delete all attached subobjects
		if (attachedObjects.GetObjectCount() > 0)
		{
			for (auto [_, subobject] : attachedObjects)
			{
				subobject->outer = nullptr; // set subobject's outer to null, so it cannot call Detach on destruction
				delete subobject;
			}
		}
        attachedObjects.RemoveAll();
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

        this->creationThreadId = Thread::GetCurrentThreadId();
        this->objectFlags = initializer->GetObjectFlags();
        if (initializer->uuid != 0)
            this->uuid = initializer->uuid;
        this->name = initializer->name;
    }

    void Object::AttachSubobject(Object* subobject)
    {
        if (subobject == nullptr)
            return;
        attachedObjects.AddObject(subobject);
        subobject->outer = this;
    }

    void Object::DetachSubobject(Object* subobject)
    {
        if (subobject == nullptr)
            return;
        subobject->outer = nullptr;
        attachedObjects.RemoveObject(subobject);
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

    void Object::RequestDestroy()
    {
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
				delete subobject;
			}
		}
		attachedObjects.RemoveAll();

        delete this;
    }

    Package* Object::GetPackage()
    {
        auto outerObject = outer;
        if (outerObject == nullptr)
            return nullptr;
        
        while (outerObject != nullptr)
        {
            if (outerObject->GetClass()->IsSubclassOf(TYPEID(Package)))
                return (Package*)outerObject;
            outerObject = outerObject->outer;
        }
        
        return nullptr;
    }

	Name Object::GetPathInPackage()
	{
        if (IsPackage())
            return "";
        
		String path = name;
		
		auto outerObject = outer;

		while (outerObject != nullptr && !outerObject->IsPackage())
		{
			auto outerPathName = outerObject->GetName();
			if (!outerPathName.IsEmpty())
				path = outerPathName + "." + path;
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
				outReferences.Add({ uuid, subobject });
			subobject->FetchObjectReferences(outReferences);
		}

		auto objectFields = GetClass()->FetchObjectFields();

		for (auto field : objectFields)
		{
			auto object = field->GetFieldValue<Object*>(this);
			if (object != nullptr)
			{
				if (!outReferences.KeyExists(object->GetUuid()))
					outReferences.Add({ object->GetUuid(), object });
			}
		}
	}

    void Object::LoadConfig(ClassType* configClass, String fileName)
    {
        if (configClass == NULL)
            configClass = GetClass();
        
        if (fileName.IsEmpty() && configClass->HasAttribute("Config"))
        {
            fileName = configClass->GetAttribute("Config").GetStringValue();
        }
        
        if (fileName.IsEmpty())
            return;
        
        auto config = gConfigCache->GetConfigFile(ConfigType(fileName));
        auto className = configClass->GetName();
        
        if (config == nullptr)
            return;
        if (!config->SectionExists(className))
            return;

        auto& configSection = config->Get(className);
        FieldType* field = configClass->GetFirstField();
        
        while (field != nullptr)
        {
            if (!field->HasAnyFieldFlags(FIELD_Config))
            {
                field = field->GetNext();
                continue;
            }
            
            auto fieldTypeId = field->GetDeclarationTypeId();
            auto fieldName = field->GetName().GetString();

            if (!configSection.KeyExists(fieldName) || !configSection[fieldName].IsValid())
            {
                field = field->GetNext();
                continue;
            }
            
            auto& configValue = configSection[fieldName];
            auto stringValue = configSection[fieldName].GetString();
            
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
            else if (field->IsArrayType() && configValue.IsValid())
            {
                
            }
            
            field = field->GetNext();
        }
    }
}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Object)

