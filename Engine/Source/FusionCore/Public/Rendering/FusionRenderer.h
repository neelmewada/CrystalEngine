#pragma once

namespace CE
{
    struct FShape;

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
        static constexpr u32 DrawItemIncrement = 1_KB;
        static constexpr u32 ShapeItemIncrement = 256;
        static constexpr u32 LineItemIncrement = 128;
        static constexpr u32 ClipItemIncrement = 128;
        static constexpr u32 CoordinateStackItemIncrement = 128;
        static constexpr u32 OpacityStackItemIncrement = 128;

        FusionRenderer();

        void Init(const FusionRendererInitInfo& initInfo);

        void OnBeforeDestroy() override;

        void Begin();

        void End();

        const Array<RHI::DrawPacket*>& FlushDrawPackets(u32 imageIndex);

        void SetViewConstants(const RPI::PerViewConstants& viewConstants);

        void SetScreenSize(Vec2i screenSize);

        void SetDrawListTag(RHI::DrawListTag drawListTag);

        // - State API -

        void SetItemTransform(const Matrix4x4& transform);

        void SetBrush(const FBrush& brush);

        void SetPen(const FPen& pen);

        void SetFont(const FFont& font);

        void PushOpacity(f32 opacity);
        void PopOpacity();

        Vec2 CalculateTextSize(const String& text, const FFont& font, f32 width = 0, FWordWrap wordWrap = FWordWrap::Normal);

        Vec2 CalculateCharacterOffsets(Array<Rect>& outRects, const String& text, const FFont& font, f32 width = 0, FWordWrap wordWrap = FWordWrap::Normal);

        FFontMetrics GetFontMetrics(const FFont& font);

        // - Draw API -

        void PushChildCoordinateSpace(const Matrix4x4& transform);
        const Matrix4x4& GetTopCoordinateSpace();
        void PopChildCoordinateSpace();

        void PushClipShape(const Matrix4x4& clipTransform, Vec2 rectSize, const FShape& shape);

        void PopClipShape();

        Vec2 DrawText(const String& text, Vec2 pos, Vec2 size = Vec2(), FWordWrap wordWrap = FWordWrap::Normal);

        void DrawLine(const Vec2& from, const Vec2& to);

    protected:

        using float4 = Vec4;
        using float2 = Vec2;
        using float4x4 = Matrix4x4;
        using uint = u32;

        // - Data Structures -

        enum FDrawType : u32
        {
	        DRAW_Shape,
            DRAW_Line,
            DRAW_Text
        };

        enum FBrushType : u32
        {
            BRUSH_None = 0,
            BRUSH_Solid,
            BRUSH_Texture,
            BRUSH_LinearGradient
        };

        enum FPenType : u32
        {
	        PEN_None = 0,
            PEN_SolidLine,
            PEN_DashedLine,
            PEN_DottedLine
        };

        struct alignas(16) FShapeItem2D
        {
            float4 cornerRadius;
            float4 brushColor;
            FBrushType brushType = BRUSH_None;
            FShapeType shape = FShapeType::None;
        };

        using FShapeItemList = StableDynamicArray<FShapeItem2D, ShapeItemIncrement, false>;

        struct alignas(8) FLineItem2D
        {
            float2 lineStart;
            float2 lineEnd;
            float dashLength = 5.0f;
        };

        using FLineItemList = StableDynamicArray<FLineItem2D, LineItemIncrement, false>;

        // Base draw item: Shape or text character
        struct alignas(16) FDrawItem2D
        {
            float4x4 transform;
            float4 penColor;
            float2 quadSize;
            float penThickness;
            float opacity;
            FDrawType drawType;
            FPenType penType = PEN_SolidLine;
            union
            {
                uint shapeIndex = 0;
                uint lineIndex;
                uint charIndex;
            };
            int clipIndex;
        };

        using FDrawItemList = StableDynamicArray<FDrawItem2D, DrawItemIncrement, false>;

        struct alignas(16) FClipItem2D
        {
            Matrix4x4 clipTransform;
            Vec4 cornerRadius;
            Vec2 size;
            FShapeType shapeType = FShapeType::Rect;
        };

        using FClipItemList = StableDynamicArray<FClipItem2D, ClipItemIncrement, false>;
        using FClipItemStack = StableDynamicArray<int, ClipItemIncrement, false>;

        using FCoordinateSpaceStack = StableDynamicArray<Matrix4x4, CoordinateStackItemIncrement, false>;
        using FOpacityStack = StableDynamicArray<f32, OpacityStackItemIncrement, false>;

        struct FDrawBatch
        {
            RHI::ShaderResourceGroup* fontSrg = nullptr;
            u32 firstDrawItemIndex = 0;
            u32 drawItemCount = 0;
        };

        // - Internal Methods -

        FDrawItem2D& DrawCustomItem(FDrawType drawType, Vec2 pos, Vec2 size);
        FDrawItem2D& DrawShape(const FShape& shape, Vec2 pos, Vec2 size);

        // - Config -

        FIELD(Config)
        u32 initialDrawItemCapacity = 5000;

        FIELD(Config)
        f32 drawItemGrowRatio = 0.25f;

        FIELD(Config)
        u32 initialClipItemCapacity = 1000;

        FIELD(Config)
        f32 clipItemGrowRatio = 0.25f;

        FIELD(Config)
        u32 initialShapeItemCapacity = 2500;

        FIELD(Config)
        f32 shapeItemGrowRatio = 0.25f;

        FIELD(Config)
        u32 initialLineItemCapacity = 500;

        // - Rendering Setup -

        u32 numFrames = 0;
        int curImageIndex = 0;
        Vec2i screenSize = Vec2i(0, 0);
        RHI::MultisampleState multisampling{};


        // - State -

        FBrush currentBrush;
        FPen currentPen;
        FFont currentFont;
        Matrix4x4 itemTransform = Matrix4x4::Identity();

        FCoordinateSpaceStack coordinateSpaceStack{};
        FOpacityStack opacityStack{};

        // - Constants -

    	RPI::PerViewConstants viewConstants{};
        StaticArray<RHI::Buffer*, MaxImageCount> viewConstantsBuffer{};
        RHI::ShaderResourceGroup* perViewSrg = nullptr;


        // - Drawing -

        RHI::ShaderResourceGroup* drawItemSrg = nullptr;

        RPI::DynamicStructuredBuffer<FDrawItem2D> drawItemsBuffer;
        FDrawItemList drawItemList;

        RPI::DynamicStructuredBuffer<FClipItem2D> clipItemsBuffer;
        FClipItemList clipItemList;
        FClipItemStack clipItemStack;

        RPI::DynamicStructuredBuffer<FShapeItem2D> shapeItemsBuffer;
        FShapeItemList shapeItemList;

        RPI::DynamicStructuredBuffer<FLineItem2D> lineItemsBuffer;
        FLineItemList lineItemList;

        Array<FDrawBatch> drawBatches{};
        bool createNewDrawBatch = false;

        // - Flush Draw Packets -

        Array<RHI::DrawPacket*> drawPackets{};
        StaticArray<bool, MaxImageCount> resubmitDrawItems{};
        StaticArray<bool, MaxImageCount> viewConstantsUpdateRequired{};
        RHI::DrawListTag drawListTag = RHI::DrawListTag::NullValue;

        // - Utils -

        RPI::Shader* fusionShader = nullptr;

        // - Cache -

        friend class FNativeContext;
        friend class FPainter;
    };
    
} // namespace CE

#include "FusionRenderer.rtti.h"
