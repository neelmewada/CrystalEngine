
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
	}

}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Object)

