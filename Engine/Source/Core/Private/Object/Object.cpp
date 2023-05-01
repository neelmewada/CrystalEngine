
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
		for (auto bus : subscribedBuses)
		{
			bus->RemoveSubscriber(this);
		}

        // Unbind all outgoing connections and remove destinations' incomingSignalBinders list
        for (auto& [name, bindingArray] : signalNameToBindingsMap)
        {
            for (const auto& binding : bindingArray)
            {
                if (binding.boundObject != nullptr)
                    binding.boundObject->incomingSignalBinders.Remove(this);
            }
        }
        signalNameToBindingsMap.Clear();
        
        // Unbind all incoming connections from 'incomingSignalBinders' because 'this' object is about to be destroyed
        for (auto incomingSignalObject : incomingSignalBinders)
        {
            incomingSignalObject->UnbindAll(this);
        }
        
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

    bool Object::Bind(Object* sourceObject, FunctionType* sourceSignal,
                      Object* destinationObject, FunctionType* destinationEvent)
    {
        if (sourceObject == destinationObject)
        {
            return false;
        }
        if (sourceObject == nullptr || destinationObject == nullptr)
        {
            return false;
        }
        
        return sourceObject->Bind(sourceSignal, destinationObject, destinationEvent);
    }

    bool Object::Bind(Name sourceSignal, Object* destinationObject, Name destinationEvent)
    {
        if (destinationObject == nullptr)
        {
            return false;
        }
        
        auto signalFunction = ((ClassType*)GetType())->FindFunctionWithName(sourceSignal);
        auto eventFunction = ((ClassType*)destinationObject->GetType())->FindFunctionWithName(destinationEvent);
        
        return Bind(signalFunction, destinationObject, eventFunction);
    }

    bool Object::Bind(FunctionType* sourceSignal, Object* destinationObject, FunctionType* destinationEvent)
    {
        if (sourceSignal == nullptr || destinationEvent == nullptr)
        {
            return false;
        }
        if (destinationObject == nullptr)
        {
            return false;
        }
        if (!sourceSignal->IsSignalFunction() || !destinationEvent->IsEventFunction())
        {
            return false;
        }
        
        SignalBinding binding{};
        binding.signalFunction = sourceSignal;
        binding.boundObject = destinationObject;
        binding.boundFunction = destinationEvent;

        if (!signalNameToBindingsMap.KeyExists(sourceSignal->GetName()))
            signalNameToBindingsMap.Add({ sourceSignal->GetName(), { binding } });
        else
            signalNameToBindingsMap[sourceSignal->GetName()].Add(binding);
        
        // flow: this -> destinationObject
        destinationObject->incomingSignalBinders.Add(this);
        
        return true;
    }

    void Object::Unbind(FunctionType* sourceSignal)
    {
        if (sourceSignal == nullptr)
        {
            return;
        }
        
        if (signalNameToBindingsMap.KeyExists(sourceSignal->GetName()))
        {
            auto& bindingArray = signalNameToBindingsMap[sourceSignal->GetName()];

            for (int i = 0; i < bindingArray.GetSize(); i++)
            {
                const auto& binding = bindingArray[i];
                if (binding.boundObject != nullptr)
                {
                    binding.boundObject->incomingSignalBinders.Remove(this);
                }
            }
            
            signalNameToBindingsMap.Remove(sourceSignal->GetName());
        }
        
        return;
    }

    void Object::UnbindAll(Object* destinationObject)
    {
        if (destinationObject == nullptr)
        {
            return;
        }
        
        Array<Name> bindingsToRemove{};
        
        for (auto& [name, bindingArray] : signalNameToBindingsMap)
        {
            for (const auto& binding : bindingArray)
            {
                if (binding.boundObject == destinationObject)
                {
                    bindingsToRemove.Add(name);
                }
            }
        }
        
        for (auto name : bindingsToRemove)
        {
            signalNameToBindingsMap.Remove(name);
        }
    }

    void Object::FireSignal(Name signalName, const CE::Array<CE::Variant>& params)
    {
        if (signalNameToBindingsMap.KeyExists(signalName))
        {
            for (const auto& binding : signalNameToBindingsMap[signalName])
            {
                binding.boundFunction->Invoke(binding.boundObject, params);
            }
        }
    }

    void Object::LoadConfig(ClassType* configClass/*=NULL*/, String fileName/*=""*/)
    {
        if (configClass == NULL)
            configClass = GetClass();
        
        if (fileName.IsEmpty())
        {
            if (configClass->HasAttribute("Config"))
            {
                fileName = configClass->GetAttribute("Config").GetStringValue();
            }
        }
        
        if (fileName.IsEmpty())
            return;
        
        auto config = gConfigCache->GetConfigFile(ConfigType(fileName));
        auto className = configClass->GetName();
        
        if (config == nullptr)
            return;
        if (!config->SectionExists(className))
            return;
        
        FieldType* field = configClass->GetFirstField();
        
        while (field != nullptr)
        {
            if (!field->HasAnyFieldFlags(FIELD_Config))
                continue;
            
            auto fieldTypeId = field->GetDeclarationTypeId();
            auto value = config->Get(className).GetString();
            
            if (fieldTypeId == TYPEID(String))
            {
                // TODO: Fetch values...
            }
            
            field = field->GetNext();
        }
    }
}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Object)

