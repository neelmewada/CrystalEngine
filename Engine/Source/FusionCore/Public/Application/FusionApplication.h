#pragma once

namespace CE
{
    class FNativeWindow;
    class FLayoutManager;

    struct FusionInitInfo
    {
        
    };

    CLASS()
    class FUSIONCORE_API FusionApplication : public Object
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

    private:

        void InitializeShaders();

        
        Array<FNativeWindow*> nativeWindows{};

        RPI::Shader* fusionShader = nullptr;

        FUSION_TESTS;
    };
    
} // namespace CE

#include "FusionApplication.rtti.h"
