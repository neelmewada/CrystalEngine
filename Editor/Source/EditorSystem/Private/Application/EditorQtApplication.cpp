
#include "CoreMinimal.h"

#include "Application/EditorQtApplication.h"
#include "GUI/Screens/WelcomeScreen/WelcomeScreen.h"
#include "GUI/Screens/ProjectBrowser/ProjectBrowser.h"

#include <QStyleFactory>
#include <QWidget>

namespace CE::Editor
{
    
    EditorQtApplication::EditorQtApplication(int argc, char** argv)
        : CEQtApplication(argc, argv)
    {
        EditorSystemEventBus::BusConnect(this);
    }

    EditorQtApplication::~EditorQtApplication()
    {
        EditorSystemEventBus::BusDisconnect(this);

        delete welcomeScreen;
        delete projectBrowser;
    }

    void EditorQtApplication::Initialize()
    {
        this->welcomeScreen = new WelcomeScreen;
        welcomeScreen->show();
    }

    void EditorQtApplication::OnWelcomeScreenTimeout()
    {
        this->projectBrowser = new ProjectBrowser;
        projectBrowser->show();
    }

    void EditorQtApplication::CreateProject(IO::Path projectDirectory, String projectName)
    {
        
    }

} // namespace CE::Editor
