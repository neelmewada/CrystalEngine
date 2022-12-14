#pragma once

#include "CEQtApplication.h"
#include "Events/EditorSystemEventBus.h"

namespace CE::Editor
{

    class EDITORSYSTEM_API EditorQtApplication
        : public Qt::CEQtApplication
    {
        Q_OBJECT

    public:

        EditorQtApplication(int argc, char** argv);

        virtual ~EditorQtApplication();


    private:

    };
    
} // namespace CE::Editor
