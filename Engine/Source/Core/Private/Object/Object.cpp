
#include "Object/Object.h"
#include "Messaging/MessageBus.h"

#include "Object/ObjectManager.h"

namespace CE
{

	Object::Object(UUID uuid) : name(""), uuid(uuid)
	{
		
	}

	Object::Object(CE::Name name, UUID uuid) : name(name), uuid(uuid)
	{
		
	}

	Object::~Object()
	{
		for (auto bus : subscribedBuses)
		{
			bus->RemoveSubscriber(this);
		}

        // Unbind all outgoing connections and remove destinations' incomingSignalBinders list
        for (auto& [name, binding] : signalBindings)
        {
            if (binding.boundObject != nullptr)
                binding.boundObject->incomingSignalBinders.Remove(this);
        }
        signalBindings.Clear();
        
        // Unbind all incoming connections from 'incomingSignalBinders' because 'this' object is about to be destroyed
        for (auto incomingSignalObject : incomingSignalBinders)
        {
            incomingSignalObject->UnbindAll(this);
        }
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
        binding.signalFuntion = sourceSignal;
        binding.boundObject = destinationObject;
        binding.boundFunction = destinationEvent;
        
        signalBindings.Add({ sourceSignal->GetName(), binding });
        
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
        
        if (signalBindings.KeyExists(sourceSignal->GetName()))
        {
            auto& binding = signalBindings[sourceSignal->GetName()];
            if (binding.boundObject != nullptr)
            {
                binding.boundObject->incomingSignalBinders.Remove(this);
            }
            signalBindings.Remove(sourceSignal->GetName());
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
        
        for (auto& [name, binding] : signalBindings)
        {
            if (binding.boundObject == destinationObject)
            {
                bindingsToRemove.Add(name);
            }
        }
        
        for (auto name : bindingsToRemove)
        {
            signalBindings.Remove(name);
        }
    }

    void Object::FireSignal(Name signalName, const CE::Array<CE::Variant>& params)
    {
        if (signalBindings.KeyExists(signalName))
        {
            auto& binding = signalBindings[signalName];
            binding.boundFunction->Invoke(binding.boundObject, params);
        }
    }
}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Object)

