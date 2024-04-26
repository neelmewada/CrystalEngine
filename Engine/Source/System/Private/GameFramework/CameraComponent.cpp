#include "System.h"

namespace CE
{

    CameraComponent::CameraComponent()
    {
		canTick = true;

#if PLATFORM_DESKTOP
        renderPipeline = CreateDefaultSubobject<MainRenderPipeline>("RenderPipeline");
#else
		#error Unimplemented render pipeline
#endif

        if (!IsDefaultInstance())
        {
            rpiView = RPI::View::CreateView("Camera", View::UsageCamera);
        }
    }

    CameraComponent::~CameraComponent()
    {
        rpiView = nullptr;
    }

    void CameraComponent::SetRenderPipeline(CE::RenderPipeline* renderPipeline)
    {
        CE::Scene* scene = GetScene();
        if (scene)
        {
            scene->RemoveRenderPipeline(this->renderPipeline);
        }
        
        this->renderPipeline = renderPipeline;
        
        if (scene)
        {
            scene->AddRenderPipeline(renderPipeline);
        }
    }

    void CameraComponent::Tick(f32 delta)
    {
	    Super::Tick(delta);
    }

} // namespace CE
