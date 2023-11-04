#include "System.h"

namespace CE
{

    CameraActor::CameraActor()
    {
		cameraComponent = CreateDefaultSubobject<CameraComponent>("CameraComponent");
		SetRootComponent(cameraComponent);
    }

} // namespace CE