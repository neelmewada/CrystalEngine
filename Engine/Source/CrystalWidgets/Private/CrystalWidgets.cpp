
#include "CrystalWidgets.h"

#include "CrystalWidgets.private.h"


namespace CE::Widgets
{
    class CrystalWidgetsModule : public CE::Module
    {
    public:
        void StartupModule() override
        {

        }

        void ShutdownModule() override
        {

        }

        void RegisterTypes() override
        {
            
        }
    };
}

CE_IMPLEMENT_MODULE(CrystalWidgets, CE::CrystalWidgetsModule)
