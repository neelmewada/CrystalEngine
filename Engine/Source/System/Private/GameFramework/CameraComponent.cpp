#include "System.h"

namespace CE
{

    CameraComponent::CameraComponent()
    {
		canTick = true;

        if (!IsDefaultInstance())
        {
            rpiView = RPI::View::CreateView("Camera", View::UsageCamera);
        }
    }

    CameraComponent::~CameraComponent()
    {
        rpiView = nullptr;
    }

    void CameraComponent::Tick(f32 delta)
    {
	    Super::Tick(delta);
    }

} // namespace CE
