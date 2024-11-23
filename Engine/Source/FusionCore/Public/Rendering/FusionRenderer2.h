#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FusionRenderer2 : public Object
    {
        CE_CLASS(FusionRenderer2, Object)
    protected:

        FusionRenderer2();

        virtual ~FusionRenderer2();

    public:

        static constexpr u32 MaxImageCount = RHI::Limits::MaxSwapChainImageCount;
        static constexpr u32 CoordinateStackItemIncrement = 128;

        void Init(const FusionRendererInitInfo& initInfo);

        void OnBeginDestroy() override;

        // - General API -

        void SetDrawListTag(RHI::DrawListTag drawListTag)
        {
            this->drawListTag = drawListTag;
        }

        void SetViewConstants(const RPI::PerViewConstants& viewConstants);

        // - State API -

        void Begin();
        void End();

        void PushChildCoordinateSpace(const Matrix4x4& transform);
        void PopChildCoordinateSpace();

        // - Draw API -



    private:

        // - Data Structures -

        using FCoordinateSpaceStack = StableDynamicArray<Matrix4x4, CoordinateStackItemIncrement, false>;

        // - Draw Data -

        RHI::DrawListTag drawListTag = RHI::DrawListTag::NullValue;

        u32 numFrames = 0;
        int curImageIndex = 0;
        Vec2i screenSize = Vec2i(0, 0);

        RPI::Shader* fusionShader = nullptr;
        RHI::MultisampleState multisampling{};

        // - State -

        FBrush currentBrush;
        FPen currentPen;
        FFont currentFont;
        Matrix4x4 itemTransform = Matrix4x4::Identity();

        FCoordinateSpaceStack coordinateSpaceStack;

        // - View Constants -

        RPI::PerViewConstants viewConstants{};
        StaticArray<RHI::Buffer*, MaxImageCount> viewConstantsBuffer{};
        RHI::ShaderResourceGroup* perViewSrg = nullptr;

    	StaticArray<bool, MaxImageCount> viewConstantsUpdateRequired{};



    };
    
} // namespace CE

#include "FusionRenderer2.rtti.h"
