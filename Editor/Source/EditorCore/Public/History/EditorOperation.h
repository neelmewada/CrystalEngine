#pragma once

namespace CE::Editor
{
    class EditorHistory;
    class EditorOperation;
    typedef Delegate<void(const Ref<EditorOperation>&)> EditorOperationDelegate;

    CLASS()
    class EDITORCORE_API EditorOperation : public Object
    {
        CE_CLASS(EditorOperation, Object)
    protected:

        EditorOperation();

    public:

        

    protected:

        WeakRef<EditorHistory> history = nullptr;
        Name bundleSavePath;

        WeakRef<Object> target;
        Array<WeakRef<Object>> targets;

        EditorOperationDelegate execute;
        EditorOperationDelegate unexecute;

        friend static void Example();

        friend class EditorHistory;
    };

} // namespace CE::Editor

#include "EditorOperation.rtti.h"