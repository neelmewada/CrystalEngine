#pragma once

namespace CE
{
    class CameraComponent;

    CLASS(Abstract)
    class SYSTEM_API RenderPipeline : public Object
    {
        CE_CLASS(RenderPipeline, Object)
    public:

        RenderPipeline();

        virtual ~RenderPipeline();

        RPI::RenderPipeline* GetRpiRenderPipeline() const { return renderPipeline; }

        virtual void ConstructPipeline();

        virtual void Tick();

        void MarkDirty() { isDirty = true; }

        void SetMainViewTag(const Name& viewTag) { mainViewTag = viewTag; }

        RHI::DrawListTag GetBuiltinDrawListTag(RPI::BuiltinDrawItemTag builtinDrawItemTag);

        void SetOwnerCamera(CameraComponent* owner) { targetCamera = owner; }

        CameraComponent* GetOwnerCamera() const { return targetCamera; }

        void ApplyChanges();
        
    protected:

        RPI::RenderPipeline* renderPipeline = nullptr;

        FIELD()
        Name mainViewTag = "MainCamera";

        FIELD()
        CameraComponent* targetCamera = nullptr;

    public:

        FIELD()
        MSAA msaa = MSAA2;

        FIELD()
        int directionalShadowResolution = 512;

    private:

        bool isDirty = true;

    };
    
} // namespace CE

#include "RenderPipeline.rtti.h"