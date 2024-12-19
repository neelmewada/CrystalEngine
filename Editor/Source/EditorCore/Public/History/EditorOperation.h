#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorOperation : public Object
    {
        CE_CLASS(EditorOperation, Object)
    protected:

        EditorOperation();

    public:

        

    protected:

        Delegate<void(Ref<EditorOperation>)> execute;
        Delegate<void(Ref<EditorOperation>)> unexecute;

    };

} // namespace CE::Editor

#include "EditorOperation.rtti.h"