#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FViewport : public FTerminalWidget
    {
        CE_CLASS(FViewport, FTerminalWidget)
    public:

        // - Public API -

        void RecreateFrameBuffer();

    protected:

        FViewport();

        ~FViewport();

        void Construct() override;

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        void OnPaint(FPainter* painter) override;

    private:

        Vec2i currentSize = Vec2i(0, 0);

        StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount> frames{};

        int imageIndex = -1;

    public: // - Fusion Properties -

        FUSION_PROPERTY(RHI::Format, ImageFormat);
        FUSION_PROPERTY(u8, SampleCount);
        FUSION_EVENT(ScriptEvent<>, OnFrameBufferRecreated);


        FUSION_WIDGET;
    };
    
}

#include "FViewport.rtti.h"
