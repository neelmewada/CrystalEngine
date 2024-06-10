#pragma once

namespace CE
{
    class FNativeContext;
    class FFusionContext;
    class FLayoutManager;

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

        void Initialize(const FusionInitInfo& initInfo);
        void PreShutdown();
        void Shutdown();

        void Tick();

        void SetRootContext(FFusionContext* context);

        void RebuildFrameGraph();

        RPI::Shader* GetFusionShader() const { return fusionShader; }

        RHI::ShaderResourceGroupLayout GetPerViewSrgLayout() const { return perViewSrgLayout; }

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

        bool rebuildFrameGraph = true;
        bool recompileFrameGraph = true;
        int curImageIndex = 0;

        FIELD()
        FFusionContext* rootContext = nullptr;

        RPI::Shader* fusionShader = nullptr;
        RHI::ShaderResourceGroupLayout perViewSrgLayout{};

        FUSION_TESTS;
    };
    
} // namespace CE

#include "FusionApplication.rtti.h"
