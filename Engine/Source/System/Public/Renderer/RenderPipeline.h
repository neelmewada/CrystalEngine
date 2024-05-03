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

        void SetRenderPipelineAsset(RenderPipelineAsset* renderPipelineAsset);

        virtual void Tick();

        void MarkDirty() { isDirty = true; }

        void SetMainViewTag(const Name& viewTag) { mainViewTag = viewTag; }

        RHI::DrawListTag GetBuiltinDrawListTag(RPI::BuiltinDrawItemTag buitinDrawItemTag);

        void SetOwnerCamera(CameraComponent* owner) { targetCamera = owner; }

        CameraComponent* GetOwnerCamera() const { return targetCamera; }
        
    protected:

        RPI::RenderPipeline* renderPipeline = nullptr;

        FIELD()
        Name mainViewTag = "MainCamera";

        FIELD()
        RenderPipelineAsset* renderPipelineAsset = nullptr;

        FIELD()
        CameraComponent* targetCamera = nullptr;

    private:

        bool isDirty = true;

    };
    
} // namespace CE

#include "RenderPipeline.rtti.h"