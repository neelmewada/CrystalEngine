
#include "Fusion.h"

#include "Fusion.private.h"

namespace CE
{
    
    class FusionModule : public CE::Module
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

} // namespace CE

CE_IMPLEMENT_MODULE(Fusion, CE::FusionModule)