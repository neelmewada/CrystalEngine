#pragma once

#include "Application/EditorQtApplication.h"
#include "Events/EditorSystemEventBus.h"
#include "Events/CrystalEditorEventBus.h"

namespace CE::Editor
{
    class WelcomeScreen;
    class ProjectBrowser;
    class CrystalEditorWindow;

    class CRYSTALEDITOR_API CrystalEditorApplication
        : public EditorQtApplication
        , public CrystalEditorEventBus::Handler
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

    private:
        WelcomeScreen* welcomeScreen = nullptr;
        ProjectBrowser* projectBrowser = nullptr;
        CrystalEditorWindow* editorWindow = nullptr;
    };
    
} // namespace CE::Editor
