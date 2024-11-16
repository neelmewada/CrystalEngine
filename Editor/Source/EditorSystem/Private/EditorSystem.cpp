
#include "EditorSystem.h"

#include "EditorSystem.private.h"


namespace CE::Editor
{
	EDITORSYSTEM_API AssetDefinitionRegistry* gAssetDefinitionRegistry = nullptr;

    EDITORSYSTEM_API EditorEngine* gEditor = nullptr;

    class EditorSystemModule : public CE::Module
    {
    public:
        virtual void StartupModule() override
        {
            gEditor = CreateObject<EditorEngine>(nullptr, "EditorEngine", OF_Transient);
            gEngine = gEditor;
        }

        virtual void ShutdownModule() override
        {
			gAssetDefinitionRegistry->BeginDestroy();
			gAssetDefinitionRegistry = nullptr;

			gEngine->BeginDestroy();
			gEngine = nullptr;
            gEditor = nullptr;
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
