
#include "Object/Object.h"

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
		
	}

}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Object)

