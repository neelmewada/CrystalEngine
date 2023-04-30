
#include "Component/Component.h"

namespace CE
{



	Component::Component(CE::Name name) : Object(name)
	{
		
	}

	Component::~Component()
	{

	}

} // namespace CE

//CE_RTTI_CLASS_IMPL(CORE_API, CE, Component)


CE::ClassType* CE::Component::Type() {
	static Internal::TypeInfoImpl<CE::Component> instance{ 
		"CE" "::" "Component", 
		&instance, sizeof(CE::Component), StructTypeData<CE::Component>() }; return &instance.Type;
}