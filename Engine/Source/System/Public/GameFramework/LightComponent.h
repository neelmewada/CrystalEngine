#pragma once

namespace CE
{
    CLASS(Abstract)
    class SYSTEM_API LightComponent : public SceneComponent
    {
        CE_CLASS(LightComponent, SceneComponent)
    public:

        LightComponent();

        virtual ~LightComponent();

    protected:

        FIELD(EditAnywhere, Category = "Light")
        Color lightColor{};

        FIELD(EditAnywhere, Category = "Light")
        f32 intensity = 1.0f;

    private:

    };
    
} // namespace CE

#include "LightComponent.rtti.h"