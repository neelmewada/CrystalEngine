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

            renderPipeline->SetOwnerCamera(this);
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
        if (renderPipeline)
        {
            renderPipeline->SetMainViewTag(viewTag);
        }
        
        if (scene)
        {
            scene->AddRenderPipeline(renderPipeline, this);
        }
    }


    void CameraComponent::Tick(f32 delta)
    {
	    Super::Tick(delta);

        if (windowSize.x > 0 && windowSize.y > 0)
        {
	        if (projection == CameraProjection::Perspective && windowSize.height > 0)
	        {
                projectionMatrix = Matrix4x4::PerspectiveProjection((f32)windowSize.width / windowSize.height, fieldOfView, nearPlane, farPlane);
	        }
            else if (projection == CameraProjection::Orthogonal && windowSize.height > 0)
            {
                projectionMatrix = Matrix4x4::OrthographicProjection((f32)windowSize.width / windowSize.height, nearPlane, farPlane);
            }

            Vec3 lookDir = GetForwardVector();
            Vec3 upDir = GetUpwardVector();
            
            viewMatrix = Quat::LookRotation2(lookDir, upDir).ToMatrix() * Matrix4x4::Translation(-GetPosition());
        }
    }

} // namespace CE
