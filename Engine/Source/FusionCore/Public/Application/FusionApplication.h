#pragma once


namespace CE
{
	class FViewport;
	class FNativeContext;
    class FFusionContext;
    class FLayoutManager;
    class FNativeContext;
    class FRootContext;
    class FStyleManager;
    class FGameWindow;

    struct IFusionAssetLoader
    {
        virtual ~IFusionAssetLoader() = default;

        virtual RHI::Texture* LoadTextureAtPath(const Name& path) = 0;

    };

    struct FusionInitInfo
    {
        IFusionAssetLoader* assetLoader = nullptr;
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

        FFontManager* GetFontManager() const { return fontManager.Get(); }

        FStyleManager* GetStyleManager() const { return styleManager.Get(); }

        FRootContext* GetRootContext() const { return rootContext.Get(); }

        f32 GetScaling() const { return scaling; }

        void Initialize(const FusionInitInfo& initInfo);
        void PreShutdown();
        void Shutdown();

        void RegisterViewport(FViewport* viewport);
        void DeregisterViewport(FViewport* viewport);

        void PushCursor(SystemCursor cursor);
        SystemCursor GetCursor();
        void PopCursor();

        int LoadImageAsset(const Name& assetPath);
        int LoadImageResource(const IO::Path& resourcePath, const Name& imageName);
        int RegisterImage(const Name& imageName, RHI::Texture* image);
        void DeregisterImage(const Name& imageName);
        void DeregisterImage(RHI::Texture* image);
        void ReplaceImage(int index, RHI::Texture* newImage);
        RHI::Texture* FindImage(const Name& imageName);
        int FindImageIndex(const Name& imageName);
        int FindImageIndex(RHI::Texture* image);

        int FindOrRegisterImage(const Name& imageName, RHI::Texture* image);

        int FindOrCreateSampler(const RHI::SamplerDescriptor& samplerDesc);

        bool IsExposed() const { return isExposed; }
        void SetExposed();
        void ResetExposed();

        void Tick();

        void RebuildFrameGraph();

        RPI::Shader* GetFusionShader() const { return fusionShader; }

        RHI::ShaderResourceGroupLayout GetPerViewSrgLayout() const { return perViewSrgLayout; }
        RHI::ShaderResourceGroupLayout GetPerObjectSrgLayout() const { return perObjectSrgLayout; }

        void QueueDestroy(Object* object);

        u64 ComputeMemoryFootprint() override;

        void EmplaceFrameAttachments();
        void EnqueueScopes();

        void UpdateDrawListMask(RHI::DrawListMask& drawListMask);

        void EnqueueDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex);

        void FlushDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex);

        ScriptEvent<void(FGameWindow*)> onRenderViewportDestroyed;

    protected:

        void OnWindowRestored(PlatformWindow* window) override;
        void OnWindowDestroyed(PlatformWindow* window) override;
        void OnWindowClosed(PlatformWindow* window) override;
        void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;
        void OnWindowMinimized(PlatformWindow* window) override;
        void OnWindowCreated(PlatformWindow* window) override;
        void OnWindowExposed(PlatformWindow* window) override;

        void OnRenderViewportDestroyed(FGameWindow* renderViewport);

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
        Ref<FRootContext> rootContext = nullptr;

        FIELD()
        Ref<FFontManager> fontManager = nullptr;

        FIELD()
        Ref<FStyleManager> styleManager = nullptr;

        IFusionAssetLoader* assetLoader = nullptr;

        StableDynamicArray<SystemCursor, 32, false> cursorStack;

        using FImageArray = StableDynamicArray<RHI::Texture*, 64, false>;
        using FTextureArray = StableDynamicArray<RPI::Texture*, 64, false>;
        using FSamplerArray = StableDynamicArray<RHI::Sampler*, 32, false>;

        FImageArray registeredImages;
        FTextureArray loadedTextures;
        HashMap<Name, int> registeredImagesByName;
        HashMap<RHI::Texture*, int> registeredImageIndices;
        HashMap<RHI::Texture*, Name> registeredImageNames;
        bool imageRegistryUpdated = true;

        FSamplerArray samplerArray;
        HashMap<RHI::Sampler*, int> samplerIndices;
        bool samplersUpdated = true;
        f32 scaling = 1.0f;

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

        HashSet<FViewport*> viewports{};

        FUSION_FRIENDS;
        friend class FTimer;
        friend class Engine;
        friend class FGameWindow;
    };
    
} // namespace CE

#include "FusionApplication.rtti.h"
