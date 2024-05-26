#pragma once

namespace CE::RPI
{
    class Shader;
    class Texture;

    enum class GradientType
    {
        Linear = 0
    };
    ENUM_CLASS(GradientType);

    struct CORERPI_API ColorGradient
    {
    public:

        struct alignas(16) Key
        {
            Vec4 color{};
            f32 position = 0;

            SIZE_T GetHash() const;
        };

        Array<Key> keys{};

        f32 degrees = 0.0f;

        SIZE_T GetHash() const;
    };

    struct Renderer2DDescriptor
    {
        Vec2i screenSize{};
        //RPI::Shader* textShader = nullptr;
        RPI::Shader* drawShader = nullptr;
        u32 numFramesInFlight = 2;
        PerViewConstants viewConstantData{};
        RHI::DrawListTag drawListTag = 0;
        RHI::MultisampleState multisampling{};

        Matrix4x4 rootTransform = Matrix4x4::Identity();
        
#if PAL_TRAIT_BUILD_EDITOR
        // Pre-allocates storage for a set number of draw items. Ex: 10,000 characters
        u32 initialDrawItemStorage = 50'000;
        u32 drawItemStorageIncrement = 50'000;
#else
        u32 initialDrawItemStorage = 10'000;
        u32 drawItemStorageIncrement = 10'000;
#endif
    };

    /**
     * \brief A utility class used for 2D rendering.
     * \details This class serves as the Rendering API for drawing 2d text and geometry.
     * The CrystalWidgets library uses this class for rendering it's 2d GUI widgets.
     * Converts 2d draw commands like DrawRect, etc into Draw Items that can be submitted to Draw Lists for execution.
     */
    class CORERPI_API Renderer2D
    {
    public:

        Renderer2D(const Renderer2DDescriptor& desc);
        virtual ~Renderer2D();

        // - Getters & Setters -

        Name GetDefaultFontName() const { return defaultFontName; }

        void SetScreenSize(Vec2i newScreenSize);

        void RegisterFont(Name name, RPI::FontAtlasAsset* fontAtlasData);

        inline void SetDefaultFont(RPI::FontAtlasAsset* fontAtlasData)
        {
            this->defaultFont = fontAtlasData;
        }

        void SetViewConstants(const PerViewConstants& viewConstants);

        inline const PerViewConstants& GetViewConstants() const { return viewConstants; }

        // - Draw API -

        void Begin();

        void ResetToDefaults();

        void PushFont(Name family, u32 fontSize = 16, bool bold = false);
        void PopFont();

        void PushClipRect(Rect clipRect, Vec4 cornerRadius = {});
        void PopClipRect();
        bool ClipRectExists();
        Rect GetLastClipRect();

        void SetFillGradient(const ColorGradient& gradient, GradientType gradientType = GradientType::Linear);
        void SetFillColor(const Color& color);
        void SetOutlineColor(const Color& color);
        void SetBorderThickness(f32 thickness);
        void SetRotation(f32 degrees);

        f32 GetRotation() const { return rotation; }

        f32 GetBorderThickness() const { return borderThickness; }

        void SetCursor(Vec2 position);

        Vec2 CalculateTextSize(const String& text, f32 width = 0.0f);
        Vec2 CalculateTextSize(const String& text, f32 fontSize, Name fontName, f32 width = 0.0f);

        Vec2 CalculateTextOffsets(Array<Rect>& outOffsetRects, const String& text, f32 width = 0.0f);
        Vec2 CalculateTextOffsets(Array<Rect>& outOffsetRects, const String& text, f32 fontSize, Name fontName, f32 width = 0.0f);

        f32 GetFontLineHeight();

        Vec2 DrawText(const String& text, Vec2 size = {});

        inline Vec2 DrawText(const Rect& rect, const String& text)
        {
            SetCursor(rect.min);
            return DrawText(text, rect.GetSize());
        }

        Vec2 DrawCircle(Vec2 size);

        inline Vec2 DrawCircle(const Rect& rect)
        {
            SetCursor(rect.min);
            return DrawCircle(rect.GetSize());
        }

        Vec2 DrawRect(Vec2 size);

        inline Vec2 DrawRect(const Rect& rect)
        {
            SetCursor(rect.min);
            return DrawRect(rect.GetSize());
        }

        Vec2 DrawRoundedRect(Vec2 size, const Vec4& cornerRadius);

        inline Vec2 DrawRoundedRect(const Rect& rect, const Vec4& cornerRadius)
        {
            SetCursor(rect.min);
            return DrawRoundedRect(rect.GetSize(), cornerRadius);
        }

        Vec2 DrawDashedLine(Vec2 size, f32 dashLength);

        Vec2 DrawDashedLine(const Rect& rect, f32 dashLength)
        {
            SetCursor(rect.min);
            return DrawDashedLine(rect.GetSize(), dashLength);
        }

        //! @brief Draws a simple isosceles triangle in the given rect
        Vec2 DrawTriangle(Vec2 size);

        Vec2 DrawTriangle(const Rect& rect)
        {
            SetCursor(rect.min);
            return DrawTriangle(rect.GetSize());
        }

        Vec2 DrawRoundedX(Vec2 size);

        Vec2 DrawRoundedX(const Rect& rect)
        {
            SetCursor(rect.min);
            return DrawRoundedX(rect.GetSize());
        }

        Vec2 DrawTexture(RPI::Texture* texture, Vec2 size);

        Vec2 DrawTexture(RPI::Texture* texture, const Rect& rect)
        {
            SetCursor(rect.min);
            return DrawTexture(texture, rect.GetSize());
        }

        Vec2 DrawTexture(RPI::Texture* texture, Vec2 size, bool repeatX, bool repeatY, Vec2 uvScale = Vec2(1, 1), Vec2 uvOffset = Vec2(0, 0));

        Vec2 DrawTexture(RPI::Texture* texture, const Rect& rect, bool repeatX, bool repeatY, Vec2 uvScale = Vec2(1, 1), Vec2 uvOffset = Vec2(0, 0))
	    {
            SetCursor(rect.min);
            return DrawTexture(texture, rect.GetSize(), repeatX, repeatY, uvScale, uvOffset);
	    }

        Vec2 DrawFrameBuffer(const StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount>& frames, Vec2 size);

        Vec2 DrawFrameBuffer(const StaticArray<RPI::Texture*, RHI::Limits::MaxSwapChainImageCount>& frames, const Rect& rect)
        {
            SetCursor(rect.min);
            return DrawFrameBuffer(frames, rect);
        }

        void End();

        const Array<DrawPacket*>& FlushDrawPackets(u32 imageIndex);

    private:

        struct DrawBatch;
        struct DrawItem2D;
        static constexpr u32 MaxImageCount = RHI::Limits::MaxSwapChainImageCount;

        enum DrawType : u32
        {
            DRAW_None = 0,
            DRAW_Text,
            DRAW_Circle,
            DRAW_Rect,
            DRAW_RoundedRect,
            DRAW_RoundedX,
            DRAW_Texture,
            DRAW_FrameBuffer,
            DRAW_Triangle,
            DRAW_DashedLine,
        };

        DrawItem2D& DrawCustomItem(DrawType drawType, Vec2 size);

        void IncrementCharacterDrawItemBuffer(u32 numCharactersToAdd = 0);
        void IncrementClipRectsBuffer(u32 numRectsToAdd = 0);
        void IncrementGradientKeysBuffer(u32 numKeysToAdd = 0);

        // - Helpers -

        inline void QueueDestroy(RHI::Buffer* buffer)
        {
            destructionQueue.Add({ .buffer = buffer });
        }

        RPI::Material* GetOrCreateMaterial(const DrawBatch& textDrawBatch);

        // - Data Structs -

        struct alignas(4) DrawDataConstants
        {
            u32 frameIndex = 0;
        };

        struct alignas(16) DrawItem2D
        {
            DrawItem2D() = default;

            DrawItem2D(const DrawItem2D&) = default;
            DrawItem2D& operator=(const DrawItem2D&) = default;

            Matrix4x4 transform{};
            Vec4 fillColor = Vec4();
            Vec4 outlineColor = Vec4();
            Vec4 cornerRadius = Vec4();
            Vec2 itemSize = Vec2(); // Item size in pixels
            float borderThickness = 0;
            union
            {
                f32 dashLength = 0;
                f32 gradientRadians;
            };
            DrawType drawType = DRAW_None;
            u32 charIndex = 0; // For character drawing
            u32 bold = 0;
            u32 clipRectIdx = 0;
            union
            {
                u32 textureIndex = 0;
                u32 gradientStartIndex;
            };
            union
            {
                u32 samplerIndex = 0;
                u32 gradientEndIndex;
            };
        };

        struct alignas(16) ClipRect2D
        {
            Rect rect{};
            Vec4 cornerRadius{};
        };

        struct FontInfo
        {
            Name fontName = "Roboto";
            u32 fontSize = 16;
            bool bold = false;
        };


        struct DrawBatch
        {
            FontInfo font{};
            u32 firstDrawItemIndex = 0;
            u32 drawItemCount = 0;

            inline SIZE_T GetMaterialHash() const
            {
                return font.fontName.GetHashValue();
            }
        };

        using MaterialHash = SIZE_T;

        struct DestroyItem
        {
            RHI::Buffer* buffer = nullptr;
            RPI::Material* material = nullptr;
            RHI::ShaderResourceGroup* srg = nullptr;
            int frameCounter = 0;
        };

        // - Config -

        //RPI::Shader* textShader = nullptr;
        RPI::Shader* drawShader = nullptr;
        RHI::DrawListTag drawListTag = 0;
        u32 numFramesInFlight = 2;
        u32 initialDrawItemStorage = 50'000;
        u32 drawItemStorageIncrement = 50'000;
        RHI::MultisampleState multisampling{};

        // - State -

        int curImageIndex = 0;
        Vec2i screenSize = Vec2i(0, 0);

        Array<FontInfo> fontStack{};

        Vec2 cursorPosition{};
        Color fillColor = Color(1, 1, 1, 1);
        Color outlineColor = Color(0, 0, 0, 0);
        Pair<int, int> currentGradient = { 0, 0 };
        float borderThickness = 0.0f;
        f32 rotation = 0;
        u32 startGradientIdx = 0;
        u32 endGradientIdx = 0;

        // - Constants -

        Matrix4x4 rootTransform = Matrix4x4::Identity();
        PerViewConstants viewConstants{};
        StaticArray<RHI::Buffer*, MaxImageCount> viewConstantsBuffer{};
        StaticArray<RHI::Buffer*, MaxImageCount> drawDataConstantsBuffer{};
        RHI::ShaderResourceGroup* perViewSrg = nullptr;
        StaticArray<bool, MaxImageCount> viewConstantsUpdateRequired{};

        // - Drawing -

        StaticArray<RHI::Buffer*, MaxImageCount> drawItemsBuffer{};
        StaticArray<RHI::Buffer*, MaxImageCount> clipRectsBuffer{};
        StaticArray<RHI::Buffer*, MaxImageCount> gradientKeysBuffer{};
        RHI::ShaderResourceGroup* drawItemSrg = nullptr;
        Array<DrawBatch> drawBatches{};
        Array<DrawItem2D> drawItems{};
        Array<RPI::Texture*> textures{};
        Array<RHI::SamplerDescriptor> samplerDescriptors{};
        Array<RHI::Sampler*> samplers{};
        Array<ClipRect2D> clipRects{};
        Array<ColorGradient::Key> gradientKeys{};

        u32 drawItemCount = 0;
        u32 clipRectCount = 0;
        u32 textureCount = 0;
        u32 gradientKeyCount = 0;
        Array<u32> clipRectStack{};
        bool createNewDrawBatch = false;

        Array<RHI::DrawPacket*> drawPackets{};
        StaticArray<bool, MaxImageCount> resubmitDrawData = {};
        StaticArray<bool, MaxImageCount> resubmitClipRects = {};
        StaticArray<bool, MaxImageCount> resubmitGradientKeys = {};
        
        // - Utils -

        Array<DestroyItem> destructionQueue{};

        Array<RHI::VertexBufferView> textQuadVertices{};
        RHI::DrawLinearArguments textQuadDrawArgs{};

        // - Cache -

        Name defaultFontName = "";
        RPI::FontAtlasAsset* defaultFont = nullptr;
        RPI::Material* defaultMaterial = nullptr;
        HashMap<Name, RPI::FontAtlasAsset*> fontAtlasesByName{};
        HashMap<MaterialHash, RPI::Material*> materials{};
        HashMap<MaterialHash, RHI::DrawPacket*> drawPacketsByMaterial{};
        HashMap<RPI::Texture*, int> textureIndices{};
        HashMap<RHI::SamplerDescriptor, u32> samplerIndices{};
        HashMap<ColorGradient, Pair<int, int>> gradientIndices{};
    };

} // namespace CE::RPI
