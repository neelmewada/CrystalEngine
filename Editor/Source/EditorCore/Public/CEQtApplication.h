#pragma once

#include "CoreMinimal.h"

#include <QApplication>

#include "QtComponents/StyleManager.h"

namespace CE::Editor::Qt
{

    class EDITORCORE_API CEQtApplication : public QApplication, public CE::Object
    {
        Q_OBJECT
        CE_CLASS(CEQtApplication, Object)
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

CE_RTTI_CLASS(EDITORCORE_API, CE::Editor::Qt, CEQtApplication,
    CE_SUPER(CE::Object),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
