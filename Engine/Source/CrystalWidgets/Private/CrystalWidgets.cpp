
#include "CrystalWidgets.h"

#include "CrystalWidgets.private.h"


namespace CE::Widgets
{
    class CrystalWidgetsModule : public CE::Module
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

CE_IMPLEMENT_MODULE(CrystalWidgets, CE::CrystalWidgetsModule)
