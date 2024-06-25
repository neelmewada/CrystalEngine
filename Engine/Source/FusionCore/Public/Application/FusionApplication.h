#pragma once

namespace CE
{
    class FNativeContext;
    class FFusionContext;
    class FLayoutManager;
    class FNativeContext;
    class FStyleManager;

    struct FusionInitInfo
    {
        
    };

    CLASS()
    class FUSIONCORE_API FusionApplication : public Object, protected ApplicationMessageHandler
    {
        CE_CLASS(FusionApplication, Object)
    public:

        FusionApplication();
        virtual ~FusionApplication();

        static FusionApplication* Get();
        static FusionApplication* TryGet();

        FFontManager* GetFontManager() const { return fontManager; }

        FStyleManager* GetStyleManager() const { return styleManager; }

        void Initialize(const FusionInitInfo& initInfo);
        void PreShutdown();
        void Shutdown();

        void PushCursor(SystemCursor cursor);
        void PopCursor();

        int LoadImageResource(const IO::Path& resourcePath, const Name& imageName);
        int RegisterImage(const Name& imageName, RHI::Texture* image);
        void DeregisterImage(const Name& imageName);
        void DeregisterImage(RHI::Texture* image);
        RHI::Texture* FindImage(const Name& imageName);

        void Tick(bool isExposed = false);

        void SetRootContext(FFusionContext* context);

        void RebuildFrameGraph();

        RPI::Shader* GetFusionShader() const { return fusionShader; }

        RHI::ShaderResourceGroupLayout GetPerViewSrgLayout() const { return perViewSrgLayout; }
        RHI::ShaderResourceGroupLayout GetPerObjectSrgLayout() const { return perObjectSrgLayout; }

        void QueueDestroy(Object* object);

    protected:

        void OnWindowRestored(PlatformWindow* window) override;
        void OnWindowDestroyed(PlatformWindow* window) override;
        void OnWindowClosed(PlatformWindow* window) override;
        void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;
        void OnWindowMinimized(PlatformWindow* window) override;
        void OnWindowCreated(PlatformWindow* window) override;
        void OnWindowExposed(PlatformWindow* window) override;

    protected:

        void InitializeShaders();

        void BuildFrameGraph();
        void CompileFrameGraph();

        void PrepareDrawList();

        bool rebuildFrameGraph = true;
        bool recompileFrameGraph = true;
        int curImageIndex = 0;
        bool isExposed = false;

        FIELD()
        FFusionContext* rootContext = nullptr;

        FIELD()
        FFontManager* fontManager = nullptr;

        FIELD()
        FStyleManager* styleManager = nullptr;

        StableDynamicArray<SystemCursor, 32, false> cursorStack;

        using FImageArray = StableDynamicArray<RHI::Texture*, 64, false>;
        using FTextureArray = StableDynamicArray<RPI::Texture*, 64, false>;

        FImageArray registeredImages;
        FTextureArray loadedTextures;
        HashMap<Name, int> registeredImagesByName;
        HashMap<RHI::Texture*, int> registeredImageIndices;
        HashMap<RHI::Texture*, Name> registeredImageNames;
        bool imageRegistryUpdated = true;

        Array<FTimer*> timers;

        RPI::Shader* fusionShader = nullptr;
        RHI::ShaderResourceGroupLayout perViewSrgLayout{};
        RHI::ShaderResourceGroupLayout perDrawSrgLayout{};
        RHI::ShaderResourceGroupLayout perObjectSrgLayout{};
        RHI::ShaderResourceGroup* textureSrg = nullptr;

        RHI::DrawListContext drawList{};

        Array<FNativeContext*> nativeWindows{};

        struct DestroyItem
        {
            Object* object = nullptr;
            int frameCounter = 0;
        };
        Array<DestroyItem> destructionQueue{};

        FUSION_FRIENDS;
        friend class FTimer;
    };
    
} // namespace CE

#include "FusionApplication.rtti.h"
