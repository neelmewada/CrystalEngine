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
        static constexpr u32 VertexArrayIncrement = 4096;
        static constexpr u32 IndexArrayIncrement = 4096;
        static constexpr u32 PathArrayIncrement = 512;
        static constexpr u32 DrawCmdArrayIncrement = 128;
        static constexpr u32 ObjectDataArrayIncrement = 128;
        static constexpr u32 ArcFastTableSize = 48;

        static constexpr int CircleAutoSegmentMin = 4;
        static constexpr int CircleAutoSegmentMax = 512;

        static constexpr f32 MinOpacity = 0.0001f;

        static constexpr u32 ColorAlphaMask = 0xff000000;

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
        // 0: East, 3: South, 6: West, 9: North, 12: East
        void PathArcToFast(const Vec2& center, float radius, int angleMinOf12, int angleMaxOf12);
        void PathRect(const Rect& rect, const Vec4& cornerRadius = {});
        void PathBezierCubicCurveTo(const Vec2& p2, const Vec2& p3, const Vec2& p4, int numSegments = 0);
        void PathQuadraticCubicCurveTo(const Vec2& p2, const Vec2& p3, int numSegments = 0);

        void PathFill(bool antiAliased = true);
        void PathStroke(bool closed = false, bool antiAliased = true);
        void PathFillStroke(bool closed = false, bool antiAliased = true);

        // - Draw API -

        void FillRect(const Rect& rect, const Vec4& cornerRadius = {}, bool antiAliased = true);
        void StrokeRect(const Rect& rect, const Vec4& cornerRadius = {}, bool antiAliased = true);

        void FillCircle(const Vec2& center, f32 radius, bool antiAliased = true);
        void StrokeCircle(const Vec2& center, f32 radius, bool antiAliased = true);

    private:

        // - Internal Draw API -

        int CalculateNumCircleSegments(float radius) const;

        void PathArcToFastInternal(const Vec2& center, float radius, int sampleMin, int sampleMax, int step);

        void PrimReserve(int vertexCount, int indexCount);
        void PrimUnreserve(int vertexCount, int indexCount);

        void PrimRect(const Rect& rect, u32 color);

        void AddDrawCmd();

        void PathInsert(const Vec2& point);
        void PathMinMax(const Vec2& point);

        void AddRect(const Rect& rect, const Vec4& cornerRadius = {}, bool antiAliased = true);
        void AddRectFilled(const Rect& rect, const Vec4& cornerRadius = {}, bool antiAliased = true);
        void AddConvexPolySolidFill(const Vec2* points, int numPoints, bool antiAliased);
        void AddPolyLine(const Vec2* points, int numPoints, f32 thickness, bool closed, bool antiAliased);
        void AddCircle(const Vec2& center, f32 radius, int numSegments, bool antiAliased);
        void AddCircleFilled(const Vec2& center, f32 radius, int numSegments, bool antiAliased);

        // - Utility API -

        void GrowQuadBuffer(u64 newTotalSize);

        void QueueDestroy(RHI::Buffer* buffer);

        // - Config -

        FIELD(Config)
        u32 initialMeshBufferSize = 24'000;

        FIELD(Config)
        f32 meshBufferGrowRatio = 0.2f;

        FIELD(Config)
        u32 initialObjectCount = 128;

        FIELD(Config)
        u32 objectDataGrowCount = 128;

        FIELD(Config)
        f32 circleSegmentMaxError = 0.3f;

        FIELD(Config)
        f32 curveTessellationTolerance = 1.25f;

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

            RHI::ShaderResourceGroup* fontSrg = nullptr;
        };

        struct FVertex
        {
            Vec2 position;
            Vec2 uv;
            u32 color = Color::White().ToU32();
            u32 drawType = 0;
        };

        struct FCoordinateSpace
        {
            Matrix4x4 transform;
            Vec2 translation;

            bool isTranslationOnly = false;
        };

        using FDrawIndex = u16;

        using FCoordinateSpaceStack = StableDynamicArray<FCoordinateSpace, CoordinateStackItemIncrement, false>;
        using FVertexArray = StableDynamicArray<FVertex, VertexArrayIncrement, false>;
        using FIndexArray = StableDynamicArray<FDrawIndex, IndexArrayIncrement, false>;
        using FPathArray = StableDynamicArray<Vec2, PathArrayIncrement, false>;
        using FDrawCmdArray = StableDynamicArray<FDrawCmd, DrawCmdArrayIncrement, false>;
        using FObjectDataArray = StableDynamicArray<FObjectData, ObjectDataArrayIncrement, false>;

        // - Draw Data -

        FVertexArray vertexArray;
        FIndexArray indexArray;
        FPathArray path;
        FPathArray temporaryPoints;
        Vec2 pathMin, pathMax;

        FVertex* vertexWritePtr = nullptr;
        FDrawIndex* indexWritePtr = nullptr;

    	// Start offset of current vertex
        FDrawIndex vertexCurrentIdx = 0;

        float fringeScale = 1.0f;

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

        Vec2 arcFastVertex[ArcFastTableSize] = {};
        float arcFastRadiusCutoff = 0;

        friend class FNativeContext;
        friend static void PathBezierCubicCurveToCasteljau(FusionRenderer2* renderer, 
            float x1, float y1, float x2, float y2, float x3, float y3, float x4, float y4, float tess_tol, int level);
        friend static void PathBezierQuadraticCurveToCasteljau(FusionRenderer2* renderer,
            float x1, float y1, float x2, float y2, float x3, float y3, float tess_tol, int level);
    };

} // namespace CE

#include "FusionRenderer2.rtti.h"