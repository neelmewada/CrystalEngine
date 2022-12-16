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
        , public CrystalEditorEventBus::Handler
        , public CE::ApplicationBus::Handler
    {
        Q_OBJECT

    public:

        CrystalEditorApplication(int argc, char** argv);

        virtual ~CrystalEditorApplication();

        void Initialize();

        ///////////////////////////////////////////
        // EditorSystemEventBus::Handler

        virtual void OnWelcomeScreenTimeout() override;

        virtual void OnOpenProject(IO::Path projectPath) override;

        virtual void OnCreateProject(IO::Path projectDirectory, String projectName) override;

        ///////////////////////////////////////////
        // CE::ApplicationBus::Handler

        virtual Engine* GetEngineRef() override;

    private:
        WelcomeScreen* welcomeScreen = nullptr;
        ProjectBrowser* projectBrowser = nullptr;
        CrystalEditorWindow* editorWindow = nullptr;

        Engine* engineRef = nullptr;
    };
    
} // namespace CE::Editor
