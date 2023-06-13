
#include "CoreWidgets.h"

#include "CoreWidgets.private.h"


namespace CE::Widgets
{
	// Globals

	COREWIDGETS_API Package* gWidgetsTransientPackage = nullptr;

	COREWIDGETS_API CStyleManager* gStyleManager = nullptr;

	COREWIDGETS_API Package* GetWidgetsTransientPackage()
	{
		return gWidgetsTransientPackage;
	}

	// Module

    class CoreWidgetsModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {
			gWidgetsTransientPackage = CreateObject<Package>(nullptr, TEXT("/CoreWidgets/Transient"), OF_Transient);

			gStyleManager = CreateObject<CStyleManager>(gWidgetsTransientPackage, TEXT("StyleManager"), OF_Transient);
        }

        virtual void ShutdownModule() override
        {
			gStyleManager->RequestDestroy();
			gStyleManager = nullptr;

			gWidgetsTransientPackage->RequestDestroy();
			gWidgetsTransientPackage = nullptr;
        }

        virtual void RegisterTypes() override
        {
			CE_REGISTER_TYPES(CVariant);
        }
    };
	
}

CE_IMPLEMENT_MODULE(CoreWidgets, CE::Widgets::CoreWidgetsModule)


