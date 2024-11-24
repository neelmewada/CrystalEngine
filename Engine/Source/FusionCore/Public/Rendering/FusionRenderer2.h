#pragma once

namespace CE
{
    CLASS(Config = Engine)
    class FUSIONCORE_API FusionRenderer2 : public Object
    {
        CE_CLASS(FusionRenderer2, Object)
    protected:

        FusionRenderer2();

        virtual ~FusionRenderer2();

    public:

        // - Constants 0

        static constexpr u32 MaxImageCount = RHI::Limits::MaxSwapChainImageCount;
        static constexpr u32 CoordinateStackItemIncrement = 128;
        static constexpr u32 VertexArrayIncrement = 1024;
        static constexpr u32 IndexArrayIncrement = 1024;
        static constexpr u32 PathArrayIncrement = 128;
        static constexpr u32 DrawCmdArrayIncrement = 128;
        static constexpr u32 ObjectDataArrayIncrement = 128;

        static constexpr f32 MinOpacity = 0.0001f;

        // - Lifecycle -

        void Init(const FusionRendererInitInfo& initInfo);

        void OnBeginDestroy() override;

        // - General API -

        void SetDrawListTag(RHI::DrawListTag drawListTag)
        {
            this->drawListTag = drawListTag;
        }

        void SetViewConstants(const RPI::PerViewConstants& viewConstants);

        const Array<RHI::DrawPacket*>& FlushDrawPackets(u32 imageIndex);

        // - State API -

        void Begin();
        void End();

        void PushChildCoordinateSpace(const Matrix4x4& transform);
        void PushChildCoordinateSpace(Vec2 translation);
        void PopChildCoordinateSpace();

        void SetPen(const FPen& pen);
        void SetBrush(const FBrush& brush);
        void SetFont(const FFont& font);

        // - Path API -

        void PathClear();
        void PathLineTo(const Vec2& point);
        void PathArcTo(const Vec2& center, float radius, float startAngle, float endAngle);
        void PathArcTo(const Vec2& center, float radius, float startAngle, float endAngle, int numSegments);
        void PathRect(const Rect& rect, const Vec4& cornerRadius = {});

        void PathFill(bool antiAliased = true);

        // - Draw API -

        void DrawRect(const Rect& rect);

    private:

        // - Internal Draw API -

        int CalculateNumCircleSegments(float radius) const;

        void PrimReserve(int vertexCount, int indexCount);
        void PrimUnreserve(int vertexCount, int indexCount);

        void PrimRect(const Rect& rect, u32 color);

        void AddDrawCmd();

        void PathInsert(const Vec2& point);

        void AddRectFilled(const Rect& rect, u32 color, const Vec4& cornerRadius = {});
        void AddConvexPolySolidFill(const Vec2* points, int numPoints, u32 color, bool antiAliased);

        // - Utility API -

        void GrowQuadBuffer(u64 newTotalSize);

        void QueueDestroy(RHI::Buffer* buffer);

        // - Config -

        FIELD(Config)
        u32 initialQuadBufferSize = 5'000;

        FIELD(Config)
        f32 quadBufferGrowRatio = 0.2f;

        FIELD(Config)
        u32 initialObjectCount = 128;

        FIELD(Config)
        u32 objectDataGrowCount = 128;

        FIELD(Config)
        f32 circleSegmentMaxError = 0.3f;

        // - Data Structures -

        struct FObjectData
        {
            Matrix4x4 transform = Matrix4x4::Identity();
        };

        struct DestroyItem
        {
            RHI::Buffer* buffer = nullptr;
            RPI::Material* material = nullptr;
            RHI::ShaderResourceGroup* srg = nullptr;
            int frameCounter = 0;
        };

        struct FDrawCmd
        {
            u32 vertexOffset = 0;
            u32 indexOffset = 0;
            u32 numIndices = 0;
            u32 firstInstance = 0;
        };

        struct FVertex
        {
            Vec2 position;
            Vec2 uv;
            u32 color = Color::White().ToU32();
        };

        struct FCoordinateSpace
        {
            Matrix4x4 transform;
            Vec2 translation;

            bool isTranslationOnly = false;
        };

        using FIndex = u16;

        using FCoordinateSpaceStack = StableDynamicArray<FCoordinateSpace, CoordinateStackItemIncrement, false>;
        using FVertexArray = StableDynamicArray<FVertex, VertexArrayIncrement, false>;
        using FIndexArray = StableDynamicArray<FIndex, IndexArrayIncrement, false>;
        using FPathArray = StableDynamicArray<Vec2, PathArrayIncrement, false>;
        using FDrawCmdArray = StableDynamicArray<FDrawCmd, DrawCmdArrayIncrement, false>;
        using FObjectDataArray = StableDynamicArray<FObjectData, ObjectDataArrayIncrement, false>;

        // - Draw Data -

        FVertexArray vertexArray;
        FIndexArray indexArray;
        FPathArray path;
        Vec2 pathMin, pathMax;

        FVertex* vertexWritePtr = nullptr;
        FIndex* indexWritePtr = nullptr;

    	// Start offset of current vertex
        FIndex vertexCurrentIdx = 0;

        FDrawCmdArray drawCmdList;
        FObjectDataArray objectDataArray;

        StaticArray<bool, MaxImageCount> quadUpdatesRequired{};

        // - Setup -

        RHI::DrawListTag drawListTag = RHI::DrawListTag::NullValue;

        u32 numFrames = 0;
        int curImageIndex = 0;
        bool pixelPerfect = true;
        Vec2i screenSize = Vec2i(0, 0);
        Vec2 whitePixelUV = Vec2(0, 0);

        RPI::Shader* fusionShader = nullptr;
        RHI::MultisampleState multisampling{};

        // - State -

        FBrush currentBrush;
        FPen currentPen;
        FFont currentFont;
        Matrix4x4 itemTransform = Matrix4x4::Identity();

        FCoordinateSpaceStack coordinateSpaceStack;
        int transformIndex = 0;

        // - View Constants -

        RPI::PerViewConstants viewConstants{};
        RHI::ShaderResourceGroup* perViewSrg = nullptr;
        RHI::ShaderResourceGroup* perObjectSrg = nullptr;
    	StaticArray<bool, MaxImageCount> viewConstantsUpdateRequired{};

        // - GPU Buffers -

        StaticArray<RHI::Buffer*, MaxImageCount> viewConstantsBuffer{};
        StaticArray<RHI::Buffer*, MaxImageCount> quadsBuffer{};
        DynamicStructuredBuffer<FObjectData> objectDataBuffer{};

        // - Draw List -

        StaticArray<Array<RHI::DrawPacket*>, MaxImageCount> drawPacketsPerImage{};

        // - Utils -

        Array<DestroyItem> destructionQueue{};
        Array<RHI::DrawPacket*> freePackets;

        friend class FNativeContext;
    };

} // namespace CE

#include "FusionRenderer2.rtti.h"
