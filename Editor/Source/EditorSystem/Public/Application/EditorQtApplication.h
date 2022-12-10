#pragma once

#include "CEQtApplication.h"


namespace CE::Editor
{

    class TestWindow;
    class WelcomeScreen;

    class EDITORSYSTEM_API EditorQtApplication
        : public Qt::CEQtApplication
    {
        Q_OBJECT

    public:

        EditorQtApplication(int argc, char** argv);

        virtual ~EditorQtApplication();

        void Initialize();

    private:
        WelcomeScreen* WelcomeScreen = nullptr;

    };
    
} // namespace CE::Editor
