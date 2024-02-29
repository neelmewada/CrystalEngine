
#include "EditorSystem.h"

#include "EditorSystem.private.h"


namespace CE::Editor
{
	EDITORSYSTEM_API AssetDefinitionRegistry* gAssetDefinitionRegistry = nullptr;

    class EditorSystemModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {
			gEngine = CreateObject<EditorEngine>(nullptr, "EditorEngine", OF_Transient);
        }

        virtual void ShutdownModule() override
        {
			gAssetDefinitionRegistry->Destroy();
			gAssetDefinitionRegistry = nullptr;

			gEngine->Destroy();
			gEngine = nullptr;
        }

        virtual void RegisterTypes() override
        {
			CE_REGISTER_TYPES(AssetDefinitionRegistry);

			gAssetDefinitionRegistry = CreateObject<AssetDefinitionRegistry>(nullptr, "AssetDefinitionRegistry");
        }
    };

    EDITORSYSTEM_API AssetDefinitionRegistry* GetAssetDefinitionRegistry()
    {
        return gAssetDefinitionRegistry;
    }
}

CE_IMPLEMENT_MODULE(EditorSystem, CE::Editor::EditorSystemModule)
