
#include "%(Name)s.h"

%(AutoRTTI_Header)s


namespace CE
{
    class %(Name)sModule : public CE::Module
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

CE_IMPLEMENT_MODULE(%(Name)s, CE::%(Name)sModule)
