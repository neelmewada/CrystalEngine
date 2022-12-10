#pragma once

#include <QApplication>

namespace CE::Editor::Qt
{

    class EDITORCORE_API CEQtApplication : public QApplication
    {
    public:

        CEQtApplication(int argc, char** argv);

        static void InitializeDpiScaling();

    };
    
} // namespace CE::Editor
