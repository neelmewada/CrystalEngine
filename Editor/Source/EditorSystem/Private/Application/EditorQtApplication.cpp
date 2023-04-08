
#include "CoreMinimal.h"

#include "Application/EditorQtApplication.h"

#include <QStyleFactory>
#include <QWidget>

namespace CE::Editor
{
    
    EditorQtApplication::EditorQtApplication(int argc, char** argv)
        : CEQtApplication(argc, argv)
    {
        
    }

    EditorQtApplication::~EditorQtApplication()
    {
        
    }

} // namespace CE::Editor

CE_RTTI_CLASS_IMPL(EDITORSYSTEM_API, CE::Editor, EditorQtApplication)
