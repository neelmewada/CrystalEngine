#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorHistory : public Object
    {
        CE_CLASS(EditorHistory, Object)
    protected:

        EditorHistory();

    public: // - Public API -

        void PerformOperation(const String& name, const Ref<Object>& target, const EditorOperationDelegate& execute, const EditorOperationDelegate& unexecute);
        void PerformOperation(const String& name, const Array<Ref<Object>>& targets, const EditorOperationDelegate& execute, const EditorOperationDelegate& unexecute);

        template<typename T>
        void PerformOperation(const String& name, const Ref<Object>& target, const CE::Name& relativeFieldPath,
            const T& initialValue, const T& newValue)
        {
            WeakRef<Object> targetRef = target;

            PerformOperation(name, target,
                [targetRef, newValue](const Ref<EditorOperation>& operation)
                {

                },
                [targetRef, initialValue](const Ref<EditorOperation>& operation)
                {

                });
        }

        void Undo();
        void Redo();

    protected: // - Internal -

        static constexpr u32 OperationStackSize = 512;
        using OperationStack = Array<Ref<EditorOperation>>;
        
        OperationStack historyStack;
        int topIndex = -1;

    };
    
}

#include "EditorHistory.rtti.h"
