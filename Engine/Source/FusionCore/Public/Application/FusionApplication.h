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

        void Tick(bool isExposed = false);

        void SetRootContext(FFusionContext* context);

        void RebuildFrameGraph();

        RPI::Shader* GetFusionShader() const { return fusionShader; }

        RHI::ShaderResourceGroupLayout GetPerViewSrgLayout() const { return perViewSrgLayout; }

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

        Array<FTimer*> timers;

        RPI::Shader* fusionShader = nullptr;
        RHI::ShaderResourceGroupLayout perViewSrgLayout{};
        RHI::ShaderResourceGroupLayout perDrawSrgLayout{};

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
