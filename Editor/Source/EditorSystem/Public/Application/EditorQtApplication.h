#pragma once

#include "CEQtApplication.h"
#include "Events/EditorSystemEventBus.h"

namespace CE::Editor
{

    class EDITORSYSTEM_API EditorQtApplication
        : public Qt::CEQtApplication
    {
        Q_OBJECT

        CE_CLASS(EditorQtApplication, Qt::CEQtApplication)

    public:

        EditorQtApplication(int argc, char** argv);

        virtual ~EditorQtApplication();


    private:

    };
    
} // namespace CE::Editor

CE_RTTI_CLASS(EDITORSYSTEM_API, CE::Editor, EditorQtApplication,
    CE_SUPER(),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
