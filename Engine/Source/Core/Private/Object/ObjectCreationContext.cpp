
#include "CoreMinimal.h"

namespace CE
{

	ObjectCreationStack::ObjectCreationStack()
	{

	}

	ObjectCreationStack::~ObjectCreationStack()
	{

	}

	static ObjectCreationContext threadedObjectContext{};

	CORE_API ObjectCreationContext* GetObjectCreationContext()
	{
		return &threadedObjectContext;
	}

} // namespace CE

