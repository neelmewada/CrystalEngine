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
        String title;

        WeakRef<Object> target;
        Array<WeakRef<Object>> targets;

        EditorOperationDelegate execute;
        EditorOperationDelegate unexecute;

        friend void Example();

        friend class EditorHistory;
    };

} // namespace CE::Editor

#include "EditorOperation.rtti.h"