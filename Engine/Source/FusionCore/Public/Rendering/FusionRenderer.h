#pragma once

namespace CE
{
    struct FusionRendererInitInfo
    {
	    RPI::Shader* fusionShader = nullptr;
        RHI::MultisampleState multisampling{};
    };

    CLASS(Config = Engine)
    class FUSIONCORE_API FusionRenderer : public Object
    {
        CE_CLASS(FusionRenderer, Object)
    public:

        static constexpr u32 MaxImageCount = RHI::Limits::MaxSwapChainImageCount;


        FusionRenderer();

        void Init(const FusionRendererInitInfo& initInfo);

        void OnBeforeDestroy() override;

        void Begin();

        void End();

        const Array<DrawPacket*>& FlushDrawPackets(u32 imageIndex);

        void SetViewConstants(const RPI::PerViewConstants& viewConstants);

        void SetRootTransform(const Matrix4x4& transform);

        void SetScreenSize(Vec2i screenSize);

        void SetDrawListTag(RHI::DrawListTag drawListTag);


    protected: // - Internal Methods -



    protected:

        // - Data Structures -

        enum DrawType : u32
        {
	        DRAW_Rect
        };

        struct alignas(16) FDrawItem2D
        {
            Matrix4x4 transform;
            DrawType drawType = DRAW_Rect;
        };

        struct FDrawBatch
        {
            u32 firstDrawItemIndex = 0;
            u32 drawItemCount = 0;
        };

        // - Config -

        FIELD(Config)
        u32 initialDrawItemCapacity = 5000;

        FIELD(Config)
        f32 drawItemGrowRatio = 0.25f;

        // - State -

        u32 numFrames = 0;
        int curImageIndex = 0;
        Vec2i screenSize = Vec2i(0, 0);

        Matrix4x4 rootTransform = Matrix4x4::Identity();
        RHI::MultisampleState multisampling{};

        // - Constants -

    	RPI::PerViewConstants viewConstants{};
        StaticArray<RHI::Buffer*, MaxImageCount> viewConstantsBuffer{};
        RHI::ShaderResourceGroup* perViewSrg = nullptr;

        // - Drawing -

        DynamicStructuredBuffer<FDrawItem2D> drawItemsBuffer;
        RHI::ShaderResourceGroup* drawItemSrg = nullptr;

        Array<FDrawItem2D> drawItemList{};
        u32 drawItemCount = 0;

        Array<FDrawBatch> drawBatches{};
        bool createNewDrawBatch = false;

        // - Flush Draw Packets -

        Array<RHI::DrawPacket*> drawPackets{};
        StaticArray<bool, MaxImageCount> resubmitDrawItems{};
        StaticArray<bool, MaxImageCount> viewConstantsUpdateRequired{};
        RHI::DrawListTag drawListTag = DrawListTag::NullValue;

        // - Utils -

        RPI::Shader* fusionShader = nullptr;

        // - Cache -

        friend class FNativeContext;
    };
    
} // namespace CE

#include "FusionRenderer.rtti.h"
