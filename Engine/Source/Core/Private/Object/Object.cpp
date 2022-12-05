
#include "Object/Object.h"

namespace CE
{

	Object::Object() : Name("")
	{

	}

	Object::Object(CE::Name name) : Name(name)
	{

	}

	Object::~Object()
	{

	}

}

CE_RTTI_CLASS_IMPL(CORE_API, CE, Object)

