#pragma once

namespace CE::RPI
{
    class Shader;

    struct Renderer2DDescriptor
    {
        Vec2i screenSize{};
        RPI::Shader* textShader = nullptr;
        u32 numFramesInFlight = 2;
        PerViewConstants viewConstantData{};
        RHI::DrawListTag drawListTag = 0;

        Matrix4x4 rootTransform = Matrix4x4::Identity();
        
#if PLATFORM_DESKTOP
        // Pre-allocates storage for a set number of text characters. Ex: 10,000 characters
        u32 initialCharacterStorage = 50'000;
        u32 characterStorageIncrement = 50'000;
#else
        u32 initialCharacterStorage = 10'000;
        u32 characterStorageIncrement = 10'000;
#endif
    };
    
    class CORERPI_API Renderer2D
    {
    public:

        Renderer2D(const Renderer2DDescriptor& desc);
        virtual ~Renderer2D();

        // - Setters -

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

        void PushFont(Name family, u32 fontSize = 16, bool bold = false);
        void PopFont();

        void SetForeground(const Color& foreground);
        void SetBackground(const Color& background);

        void SetCursor(Vec2 position);

        Vec2 CalculateTextSize(const String& text);

        Vec2 DrawText(const String& text, Vec2 size = {});

        void End();

        const Array<DrawPacket*>& FlushDrawPackets(u32 imageIndex);

    private:

        struct TextDrawRequest;
        struct TextDrawBatch;
        static constexpr u32 MaxImageCount = RHI::Limits::MaxSwapChainImageCount;

        void IncrementCharacterDrawItemBuffer(u32 numCharactersToAdd = 0);

        // - Helpers -

        inline void QueueDestroy(RHI::Buffer* buffer)
        {
            destructionQueue.Add({ .buffer = buffer, .imageIndex = curImageIndex });
        }

        RPI::Material* GetOrCreateMaterial(const TextDrawBatch& textDrawBatch);

        // - Data Structs -

        struct alignas(16) CharacterDrawItem
        {
            Matrix4x4 transform{};
            f32 bgMask = 0;
            u32 charIndex = 0;
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

        struct TextDrawBatch
        {
            FontInfo font{};
            Color foreground{};
            Color background{};
            u32 firstCharDrawItemIndex = 0;
            u32 charDrawItemCount = 0;

            inline SIZE_T GetMaterialHash() const
            {
                SIZE_T hash = font.fontName.GetHashValue();
                CombineHash(hash, font.bold);
                CombineHash(hash, foreground);
                CombineHash(hash, background);
                return hash;
            }
        };

        struct SDFDrawBatch
        {
            Color background{};
            Color border{};
        };

        enum DrawBatchType
        {
            DRAW_None = 0,
            DRAW_Text,
            DRAW_SDF,
        };

        struct DrawBatch
        {
            DrawBatchType drawType = DRAW_None;
            union {
                TextDrawBatch textDraw;
                
            };
        };

        using MaterialHash = SIZE_T;

        struct DestroyItem
        {
            RHI::Buffer* buffer = nullptr;
            RPI::Material* material = nullptr;
            RHI::ShaderResourceGroup* srg = nullptr;
            int imageIndex = 0;
        };

        // - Config -

        RPI::Shader* textShader = nullptr;
        RHI::DrawListTag drawListTag = 0;
        u32 numFramesInFlight = 2;
        u32 initialCharacterStorage = 50'000;
        u32 characterStorageIncrement = 50'000;

        // - State -

        int curImageIndex = 0;
        Vec2i screenSize = Vec2i(0, 0);

        Array<FontInfo> fontStack{};

        Vec2 cursorPosition{};
        Color foreground = Color(1, 1, 1, 1);
        Color background = Color(0, 0, 0, 0);

        // - Constants -

        Matrix4x4 rootTransform = Matrix4x4::Identity();
        PerViewConstants viewConstants{};
        StaticArray<RHI::Buffer*, MaxImageCount> viewConstantsBuffer{};
        RHI::ShaderResourceGroup* perViewSrg = nullptr;
        StaticArray<bool, MaxImageCount> viewConstantsUpdateRequired{};

        // - Character Drawing -

        StaticArray<RHI::Buffer*, MaxImageCount> charDrawItemsBuffer{};
        RHI::ShaderResourceGroup* charDrawItemSrg = nullptr;
        Array<TextDrawBatch> textDrawBatches{};
        Array<CharacterDrawItem> charDrawItems{};
        u32 charDrawItemsCount = 0;
        bool createNewTextBatch = false;

        Array<RHI::DrawPacket*> drawPackets{};
        bool rebuildPackets = true;
        
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

    };

} // namespace CE::RPI
