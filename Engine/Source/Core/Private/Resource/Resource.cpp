#include "Core.h"

namespace CE
{

	Resource::Resource()
	{
	}

	Resource::~Resource()
	{
		if (data != nullptr)
		{
			free(data);
			data = nullptr;
		}
	}
    
} // namespace CE

CE_RTTI_CLASS_IMPL(CORE_API, CE, Resource)
