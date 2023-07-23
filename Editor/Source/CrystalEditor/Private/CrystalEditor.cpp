
#include "CrystalEditor.h"

#include "CrystalEditor.private.h"

namespace CE
{
    class CrystalEditorModule : public CE::Module
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

CE_IMPLEMENT_MODULE(CrystalEditor, CE::CrystalEditorModule)
