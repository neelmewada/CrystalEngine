
#include "Application/EditorQtApplication.h"

#include "GUI/WelcomeScreen/WelcomeScreen.h"
#include "GUI/TestWindow/testwindow.h"

#include <QStyleFactory>

namespace CE::Editor
{
    
    EditorQtApplication::EditorQtApplication(int argc, char** argv)
        : CEQtApplication(argc, argv)
    {
        
    }

    EditorQtApplication::~EditorQtApplication()
    {
        delete WelcomeScreen;
        WelcomeScreen = nullptr;
    }

    void EditorQtApplication::Initialize()
    {
        this->WelcomeScreen = new CE::Editor::WelcomeScreen;
        WelcomeScreen->show();
    }

} // namespace CE::Editor
