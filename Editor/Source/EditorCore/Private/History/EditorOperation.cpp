#include "EditorCore.h"

namespace CE::Editor
{

    EditorOperation::EditorOperation()
    {

    }

    void Example()
    {
        // Operation scenarios:
        // - Delete component from an actor
        // - Delete actor
        // - Delete asset
        // - Create a component and add it to an actor
        // - Create an actor and attach it as a child to another actor
        // - Modify field of a class/struct

        Name operation = "Delete Object";

        auto execute = [](const Ref<EditorOperation>& self)
            {
                // Serialize the object to disk before deleting

                self->target->BeginDestroy();
            };

        auto unexecute = [](const Ref<EditorOperation>& self)
            {
                // Deserialize the object back
            };
    }
    
} // namespace CE::Editor
