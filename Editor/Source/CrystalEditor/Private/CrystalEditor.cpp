
#include "CrystalEditor.h"

namespace CE
{
    class CrystalEditorModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {
			GetStyleManager()->AddResourceSearchModule(MODULE_NAME);
        }

        virtual void ShutdownModule() override
        {
			GetStyleManager()->RemoveResourceSearchModule(MODULE_NAME);
        }

        virtual void RegisterTypes() override
        {

        }
    };
}

#include "CrystalEditor.private.h"
#include "Resource.h"
CE_IMPLEMENT_MODULE(CrystalEditor, CE::CrystalEditorModule)
