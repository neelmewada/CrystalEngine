
#include "System.h"


namespace CE
{
	SYSTEM_API AssetDefinitionRegistry* gAssetDefinitionRegistry = nullptr;

    class SystemModule : public Module
    {
    public:

        void StartupModule() override
        {
			
        }

        void ShutdownModule() override
        {
			gAssetDefinitionRegistry->RequestDestroy();
			gAssetDefinitionRegistry = nullptr;

        }

        void RegisterTypes() override
        {
			gAssetDefinitionRegistry = CreateObject<AssetDefinitionRegistry>(nullptr, "AssetDefinitionRegistry");
        }

    };

	/*
	*	Globals
	*/

	SYSTEM_API Engine* gEngine = nullptr;
    
} // namespace CE


#include "System.private.h"
CE_IMPLEMENT_MODULE(System, CE::SystemModule)

