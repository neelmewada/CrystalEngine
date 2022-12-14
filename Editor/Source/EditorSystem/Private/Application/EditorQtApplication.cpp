
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
