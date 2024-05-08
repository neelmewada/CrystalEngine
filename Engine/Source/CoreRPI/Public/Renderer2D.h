#pragma once

namespace CE::RPI
{
    class Shader;
    class Texture;

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
        
#if PLATFORM_DESKTOP
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

        void PushClipRect(Rect clipRect);
        void PopClipRect();
        bool ClipRectExists();
        Rect GetLastClipRect();

        void SetFillColor(const Color& color);
        void SetOutlineColor(const Color& color);
        void SetBorderThickness(f32 thickness);
        void SetRotation(f32 degrees);

        void SetCursor(Vec2 position);

        Vec2 CalculateTextSize(const String& text, f32 width = 0.0f);
        Vec2 CalculateTextSize(const String& text, f32 fontSize, Name fontName, f32 width = 0.0f);

        Vec2 CalculateTextOffsets(Array<Rect>& outOffsetRects, const String& text, f32 width = 0.0f);
        Vec2 CalculateTextOffsets(Array<Rect>& outOffsetRects, const String& text, f32 fontSize, Name fontName, f32 width = 0.0f);

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

        Vec2 DrawArrow(Vec2 size, f32 thickness);

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

        void End();

        const Array<DrawPacket*>& FlushDrawPackets(u32 imageIndex);

    private:

        struct DrawBatch;
        static constexpr u32 MaxImageCount = RHI::Limits::MaxSwapChainImageCount;

        void IncrementCharacterDrawItemBuffer(u32 numCharactersToAdd = 0);
        void IncrementClipRectsBuffer(u32 numRectsToAdd = 0);

        // - Helpers -

        inline void QueueDestroy(RHI::Buffer* buffer)
        {
            destructionQueue.Add({ .buffer = buffer });
        }

        RPI::Material* GetOrCreateMaterial(const DrawBatch& textDrawBatch);

        // - Data Structs -

        enum DrawType : u32
        {
            DRAW_None = 0,
            DRAW_Text,
            DRAW_Circle,
            DRAW_Rect,
            DRAW_RoundedRect,
            DRAW_RoundedX,
            DRAW_Texture,
        };

        struct alignas(16) DrawItem2D
        {
            Matrix4x4 transform{};
            Vec4 fillColor = Vec4();
            Vec4 outlineColor = Vec4();
            Vec4 cornerRadius = Vec4();
            Vec2 itemSize = Vec2(); // Item size in pixels
            float borderThickness = 0;
            DrawType drawType = DRAW_None;
            u32 charIndex = 0; // For character drawing
            u32 bold = 0;
            u32 clipRectIdx = 0;
            u32 textureIndex = 0;
        };

        struct FontInfo
        {
            Name fontName = "Roboto";
            u32 fontSize = 16;
            bool bold = false;
        };

        struct TextDrawRequest
        {
            String text{};
            Vec2 position{};
            Vec2 size{};
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
        float borderThickness = 0.0f;
        f32 rotation = 0;

        // - Constants -

        Matrix4x4 rootTransform = Matrix4x4::Identity();
        PerViewConstants viewConstants{};
        StaticArray<RHI::Buffer*, MaxImageCount> viewConstantsBuffer{};
        RHI::ShaderResourceGroup* perViewSrg = nullptr;
        StaticArray<bool, MaxImageCount> viewConstantsUpdateRequired{};

        // - Drawing -

        StaticArray<RHI::Buffer*, MaxImageCount> drawItemsBuffer{};
        StaticArray<RHI::Buffer*, MaxImageCount> clipRectsBuffer{};
        RHI::ShaderResourceGroup* drawItemSrg = nullptr;
        Array<DrawBatch> drawBatches{};
        Array<DrawItem2D> drawItems{};
        Array<RPI::Texture*> textures{};
        Array<Rect> clipRects{};
        u32 drawItemCount = 0;
        u32 clipRectCount = 0;
        u32 textureCount = 0;
        Array<u32> clipRectStack{};
        bool createNewDrawBatch = false;

        Array<RHI::DrawPacket*> drawPackets{};
        StaticArray<bool, MaxImageCount> resubmitDrawData = {};
        StaticArray<bool, MaxImageCount> resubmitClipRects = {};
        
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
    };

} // namespace CE::RPI
