#include "EditorCore.h"

namespace CE::Editor
{

    EditorOperation::EditorOperation()
    {

    }

    static void Example()
    {
        // Operation scenarios:
        // - Delete component from an actor
        // - Delete actor
        // - Delete asset
        // - Create a component and add it to an actor
        // - Create an actor and attach it as a child to another actor
        // - Modify field of a class/struct

        Name operation = "Create Object";

        auto execute = [](Ref<EditorOperation> self)
            {

            };

        auto unexecute = [](Ref<EditorOperation> self)
            {

            };
    }
    
} // namespace CE::Editor
