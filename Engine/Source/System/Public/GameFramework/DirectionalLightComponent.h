#pragma once

namespace CE
{
    CLASS()
    class SYSTEM_API DirectionalLightComponent : public LightComponent
    {
        CE_CLASS(DirectionalLightComponent, LightComponent)
    public:

        DirectionalLightComponent();

    protected:


    private:

    };
    
} // namespace CE

#include "DirectionalLightComponent.rtti.h"