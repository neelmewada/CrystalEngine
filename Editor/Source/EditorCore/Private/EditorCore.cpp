
#include "EditorCore.h"

namespace CE::Editor
{

    class EditorCoreModule : public Module
    {
    public:

        void StartupModule() override
        {
            auto app = FusionApplication::TryGet();

            if (app)
            {
                FStyleManager* styleManager = app->GetStyleManager();

                FRootContext* rootContext = app->GetRootContext();

                FStyleSet* rootStyle = CreateObject<FStyleSet>(rootContext, "RootStyleSet");
                styleManager->RegisterStyleSet(rootStyle->GetName(), rootStyle);
                rootContext->SetDefaultStyleSet(rootStyle);


            }
        }

        void ShutdownModule() override
        {
			if (ProjectManager::TryGet())
			{
                ProjectManager::TryGet()->Destroy();
			}
        }

        void RegisterTypes() override
        {

        }

        void DeregisterTypes() override
        {

        }
    };

} // namespace CE

#include "EditorCore.private.h"

CE_IMPLEMENT_MODULE(EditorCore, CE::Editor::EditorCoreModule)

