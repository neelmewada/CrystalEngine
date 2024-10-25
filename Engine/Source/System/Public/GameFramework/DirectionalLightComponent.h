#pragma once

namespace CE
{
    CLASS()
    class SYSTEM_API DirectionalLightComponent : public LightComponent
    {
        CE_CLASS(DirectionalLightComponent, LightComponent)
    public:

        DirectionalLightComponent();

        ~DirectionalLightComponent();

    protected:

        void OnEnabled() override;

        void OnDisabled() override;

        void Tick(f32 delta) override;

        // - Properties -

        FIELD(EditAnywhere, Category = "Light")
        f32 shadowDistance = 10;

        // - Internal -

        RPI::DirectionalLightHandle lightHandle{};
    };
    
} // namespace CE

#include "DirectionalLightComponent.rtti.h"