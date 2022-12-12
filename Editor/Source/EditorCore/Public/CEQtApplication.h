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

    private:
        StyleManager StyleManager{ this };
    };
    
} // namespace CE::Editor
