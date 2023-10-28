#pragma once

namespace CE
{
    CLASS()
    class SYSTEM_API CameraActor : public Actor
    {
        CE_CLASS(CameraActor, Actor)
    public:

        CameraActor();

		inline CameraComponent* GetCameraComponent() const { return cameraComponent; }

		inline void SetCameraComponent(CameraComponent* cameraComponent) { this->cameraComponent = cameraComponent; }

    protected:

		FIELD()
		CameraComponent* cameraComponent = nullptr;
    };

} // namespace CE

#include "CameraActor.rtti.h"
