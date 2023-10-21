#include "System.h"

namespace CE
{

	Actor::Actor()
	{
		rootComponent = CreateDefaultSubobject<SceneComponent>(TEXT("RootComponent"));
	}

} // namespace CE
