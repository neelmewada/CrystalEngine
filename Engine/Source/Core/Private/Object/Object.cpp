
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
        CE_ASSERT(initializer != nullptr, "An object was contructed without any initializers set! This usually happens when you construct an object using 'new' operator.");
        ObjectThreadContext::Get().PopInitializer();
        ConstructInternal(initializer);
    }

    void Object::ConstructInternal(ObjectInitializer* initializer)
    {
        CE_ASSERT(initializer != nullptr, "An object was contructed without any initializers set! This usually happens when you construct an object using 'new' operator.");
        CE_ASSERT(initializer->objectClass != nullptr, "Object initializer passed with null objectClass!");
        
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
            if (field->IsIntegerField())
            {

            }
            else if (field->IsArrayType() && configValue.IsValid())
            {
                
            }
            
            field = field->GetNext();
        }
    }
}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Object)

