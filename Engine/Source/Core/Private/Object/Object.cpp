
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
        for (auto [_, subobject] : attachedObjects)
        {
            delete subobject;
        }
        attachedObjects.Clear();
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
        subobject->ownerPackage = this->ownerPackage;
    }

    void Object::DetachSubobject(Object* subobject)
    {
        if (subobject == nullptr)
            return;
        subobject->ownerPackage = nullptr;
        subobject->outer = nullptr;
        attachedObjects.RemoveObject(subobject);
    }

    void Object::RequestDestroy()
    {
        delete this;
    }

    void Object::LoadConfig(ClassType* configClass/*=NULL*/, String fileName/*=""*/)
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

