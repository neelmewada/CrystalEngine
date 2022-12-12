#pragma once

#include "CEQtApplication.h"
#include "Events/EditorSystemEventBus.h"

namespace CE::Editor
{

    class TestWindow;
    class WelcomeScreen;
    class ProjectBrowser;

    class EDITORSYSTEM_API EditorQtApplication
        : public Qt::CEQtApplication
        , public EditorSystemEventBus::Handler
    {
        Q_OBJECT

    public:

        EditorQtApplication(int argc, char** argv);

        virtual ~EditorQtApplication();

        void Initialize();

        ///////////////////////////////////////////
        // EditorSystemEventBus::Handler

        virtual void OnWelcomeScreenTimeout() override;

        virtual void CreateProject(IO::Path projectDirectory, String projectName) override;

        ///////////////////////////////////////////

    private:
        WelcomeScreen* welcomeScreen = nullptr;
        ProjectBrowser* projectBrowser = nullptr;

    };
    
} // namespace CE::Editor
