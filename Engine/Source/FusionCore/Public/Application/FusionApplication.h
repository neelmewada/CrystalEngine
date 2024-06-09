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
        void Shutdown();

        void Tick();

        void SetRootContext(FFusionContext* context);

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


        FIELD()
        FFusionContext* rootContext = nullptr;

        RPI::Shader* fusionShader = nullptr;

        FUSION_TESTS;
    };
    
} // namespace CE

#include "FusionApplication.rtti.h"
