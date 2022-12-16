
#include "CoreMinimal.h"

#include "Application/EditorQtApplication.h"
#include "GUI/Screens/WelcomeScreen/WelcomeScreen.h"
#include "GUI/Screens/ProjectBrowser/ProjectBrowser.h"

#include "Editor/CrystalEditor/CrystalEditorWindow.h"

#include <QStyleFactory>
#include <QWidget>

namespace CE::Editor
{
    
    CrystalEditorApplication::CrystalEditorApplication(int argc, char** argv)
        : EditorQtApplication(argc, argv)
    {
        EditorSystemEventBus::BusConnect(this);
        CrystalEditorEventBus::BusConnect(this);
        ApplicationBus::BusConnect(this);

        engineRef = new Engine;
    }

    CrystalEditorApplication::~CrystalEditorApplication()
    {
        ApplicationBus::BusDisconnect(this);
        CrystalEditorEventBus::BusDisconnect(this);
        EditorSystemEventBus::BusDisconnect(this);

        delete engineRef;

        delete welcomeScreen;
        delete projectBrowser;
        delete editorWindow;
    }

    void CrystalEditorApplication::Initialize()
    {
        this->welcomeScreen = new WelcomeScreen;
        welcomeScreen->show();
    }

    void CrystalEditorApplication::OnWelcomeScreenTimeout()
    {
        this->projectBrowser = new ProjectBrowser;
        projectBrowser->show();
    }

    void CrystalEditorApplication::OnOpenProject(IO::Path projectPath)
    {
        editorWindow = new CrystalEditorWindow;
        editorWindow->show();

        if (projectBrowser != nullptr && projectBrowser->isVisible())
        {
            projectBrowser->close();
            CE_DELETE(projectBrowser);
        }
    }

    void CrystalEditorApplication::OnCreateProject(IO::Path projectDirectory, String projectName)
    {

    }

    Engine* CrystalEditorApplication::GetEngineRef()
    {
        if (engineRef == nullptr)
        {
            engineRef = new Engine;
        }
        return engineRef;
    }

} // namespace CE::Editor
