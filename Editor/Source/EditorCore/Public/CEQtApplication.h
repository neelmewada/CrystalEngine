#pragma once

#include <QApplication>

#include "QtComponents/StyleManager.h"

namespace CE::Editor::Qt
{

    class EDITORCORE_API CEQtApplication : public QApplication
    {
    public:

        CEQtApplication(int argc, char** argv);

        virtual ~CEQtApplication();

        static void InitializeDpiScaling();

        inline int GetArgumentCount() { return argc; }
        
        inline char** GetArguments() { return argv; }

    private:
        int argc;
        char** argv;

        StyleManager StyleManager{ this };
    };
    
} // namespace CE::Editor
