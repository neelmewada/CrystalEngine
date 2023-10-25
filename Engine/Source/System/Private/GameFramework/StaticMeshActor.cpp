#include "System.h"

namespace CE
{

    StaticMeshActor::StaticMeshActor()
    {
		meshComponent = CreateDefaultSubobject<StaticMeshComponent>(TEXT("StaticMeshComponent"));
		SetRootComponent(meshComponent);
    }

} // namespace CE
