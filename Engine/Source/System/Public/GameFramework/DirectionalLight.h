#pragma once

namespace CE
{
    CLASS()
    class SYSTEM_API DirectionalLight : public Light
    {
        CE_CLASS(DirectionalLight, Light)
    public:

        DirectionalLight();

    protected:

        FIELD()
        DirectionalLightComponent* directionalLightComponent = nullptr;

    };
    
} // namespace CE

#include "DirectionalLight.rtti.h"