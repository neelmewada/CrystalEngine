#include "System.h"

namespace CE
{

    DirectionalLightComponent::DirectionalLightComponent()
    {
	    
    }

    void DirectionalLightComponent::Tick(f32 delta)
    {
	    Super::Tick(delta);

        CE::Scene* scene = GetScene();
        if (!scene)
            return;

        RPI::Scene* rpiScene = scene->GetRpiScene();
        if (!rpiScene)
            return;

        DirectionalLightFeatureProcessor* fp = rpiScene->GetFeatureProcessor<DirectionalLightFeatureProcessor>();
        if (!fp)
            return;


    }

} // namespace CE
