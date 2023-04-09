
#include "CoreMinimal.h"
#include "RHI/RHI.h"

#include "EditorSystem.h"

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
        CE_CONNECT(CrystalEditorEventBus, this);
        CE_CONNECT(ApplicationBus, this);

        engineRef = new Engine;
    }

    CrystalEditorApplication::~CrystalEditorApplication()
    {
        CE_DISCONNECT(CrystalEditorEventBus, this);
        CE_DISCONNECT(ApplicationBus, this);
        EditorSystemEventBus::BusDisconnect(this);

        delete engineRef;

        delete welcomeScreen;
        delete projectBrowser;
        delete editorWindow;
    }

    void CrystalEditorApplication::Initialize()
    {
        //this->welcomeScreen = new WelcomeScreen;
        //welcomeScreen->show();
        this->projectBrowser = new ProjectBrowser;
        projectBrowser->show();
    }

    void CrystalEditorApplication::OnWelcomeScreenTimeout()
    {
        this->projectBrowser = new ProjectBrowser;
        projectBrowser->show();
    }

    void CrystalEditorApplication::OnOpenProject(IO::Path projectPath)
    {
        ProjectManager::Get().LoadProject(projectPath);

        editorWindow = new CrystalEditorWindow;
        editorWindow->show();

        if (projectBrowser != nullptr && projectBrowser->isVisible())
        {
            projectBrowser->close();
            CE_DELETE(projectBrowser);
        }

        EditorPrefs::Get().OnProjectChanged();
    }

    void CrystalEditorApplication::OnCreateProject(IO::Path projectDirectory, String projectName)
    {
        ProjectManager::Get().CreateProject(projectDirectory, projectName);

        editorWindow = new CrystalEditorWindow;
        editorWindow->show();

        if (projectBrowser != nullptr && projectBrowser->isVisible())
        {
            projectBrowser->close();
            CE_DELETE(projectBrowser);
        }

        EditorPrefs::Get().OnProjectChanged();
    }

    void CrystalEditorApplication::GetEngineRef(Engine** outEngineRef)
    {
        if (outEngineRef == nullptr)
            return;
        if (engineRef == nullptr)
        {
            engineRef = new Engine;
        }
        *outEngineRef = engineRef;
    }

} // namespace CE::Editor

