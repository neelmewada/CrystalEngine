#pragma once

namespace CE
{

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

        RPI::Shader* renderingShader = nullptr;
    };
    
} // namespace CE

#include "FusionApplication.rtti.h"
