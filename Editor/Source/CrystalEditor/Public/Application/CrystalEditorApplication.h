#pragma once

#include "Application/EditorQtApplication.h"
#include "Events/EditorSystemEventBus.h"
#include "Events/CrystalEditorEventBus.h"

#include "Application/ApplicationBus.h"

#include "GameFramework/GameFramework.h"

namespace CE::Editor
{
    class WelcomeScreen;
    class ProjectBrowser;
    class CrystalEditorWindow;

    class CRYSTALEDITOR_API CrystalEditorApplication
        : public EditorQtApplication
        , public CrystalEditorEventBus::Interface
        , public ApplicationBus::Interface
    {
        Q_OBJECT

        CE_CLASS(CrystalEditorApplication, EditorQtApplication)

    public:

        CrystalEditorApplication(int argc, char** argv);

        virtual ~CrystalEditorApplication();

        void Initialize();

        ///////////////////////////////////////////
        // EditorSystemEventBus::Interface

        virtual void OnWelcomeScreenTimeout() override;

        virtual void OnOpenProject(IO::Path projectPath) override;

        virtual void OnCreateProject(IO::Path projectDirectory, String projectName) override;

        ///////////////////////////////////////////
        // CE::ApplicationBus::Interface

        virtual void GetEngineRef(Engine** outEngineRef) override;


    private:

        WelcomeScreen* welcomeScreen = nullptr;
        ProjectBrowser* projectBrowser = nullptr;
        CrystalEditorWindow* editorWindow = nullptr;

        Engine* engineRef = nullptr;
    };
    
} // namespace CE::Editor

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, CrystalEditorApplication,
    CE_SUPER(CE::Editor::EditorQtApplication),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST(
        // EditorSystemEventBus::Interface
        CE_FUNCTION(OnWelcomeScreenTimeout, Event)
        CE_FUNCTION(OnOpenProject, Event)
        CE_FUNCTION(OnCreateProject, Event)

        // CE::ApplicationBus::Interface
        CE_FUNCTION(GetEngineRef, Event)
    )
)
