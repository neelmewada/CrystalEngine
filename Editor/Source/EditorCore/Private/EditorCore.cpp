
#include "EditorCore.h"

namespace CE::Editor
{
    EditorStyle* gEditorStyle = nullptr;


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

                gEditorStyle = CreateObject<EditorStyle>(nullptr, "RootEditorStyle");
                gEditorStyle->InitializeDefault();

                styleManager->RegisterStyleSet(gEditorStyle->GetName(), gEditorStyle);
                rootContext->SetDefaultStyleSet(gEditorStyle);
            }
        }

        void ShutdownModule() override
        {
            if (gEditorStyle)
            {
                gEditorStyle->Destroy();
                gEditorStyle = nullptr;
            }

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

