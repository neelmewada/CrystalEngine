
#include "Object/Object.h"

#include "Object/ObjectManager.h"

namespace CE
{

	Object::Object() : name("")
	{
		ObjectManager::RegisterObject(this);
	}

	Object::Object(CE::Name name) : name(name)
	{
		ObjectManager::RegisterObject(this);
	}

	Object::~Object()
	{
		ObjectManager::DeregisterObject(this);
	}

}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Object)

