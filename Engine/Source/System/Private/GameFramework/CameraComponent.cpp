#include "System.h"

namespace CE
{

    CameraComponent::CameraComponent()
    {
		canTick = true;
    }

	void CameraComponent::SetMainCamera(bool set)
	{
		isMainCamera = set;

		if (isMainCamera && GetScene() != nullptr)
		{
			GetScene()->mainCamera = this;
		}
	}

} // namespace CE
