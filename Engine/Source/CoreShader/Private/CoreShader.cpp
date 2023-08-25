
#include "CoreShader.h"




namespace CE
{
    class CoreShaderModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {

        }

        virtual void ShutdownModule() override
        {

        }

        virtual void RegisterTypes() override
        {

        }
    };
}

CE_IMPLEMENT_MODULE(CoreShader, CE::CoreShaderModule)
