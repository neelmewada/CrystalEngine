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

        void PerformOperation(const String& name, const Ref<Object>& target, 
            const EditorOperationDelegate& execute, 
            const EditorOperationDelegate& unexecute,
            const SubClass<EditorOperation>& operationClass = EditorOperation::StaticClass());

        void PerformOperation(const String& name, const Array<Ref<Object>>& targets, 
            const EditorOperationDelegate& execute, 
            const EditorOperationDelegate& unexecute,
            const SubClass<EditorOperation>& operationClass = EditorOperation::StaticClass());

        template<typename T>
        void PerformOperation(const String& name, const Ref<Object>& target, const CE::Name& relativeFieldPath,
            const T& initialValue, const T& newValue)
        {
            WeakRef<Object> targetRef = target;

            PerformOperation(name, target,
                [targetRef, relativeFieldPath, newValue](const Ref<EditorOperation>& operation)
                {
                    if (auto target = targetRef.Lock())
                    {
                        Ptr<FieldType> field;
                        void* instance = nullptr;
                        bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target,
                            field, instance);
                        if (!success)
                        {
                            return false;
                        }
                        field->SetFieldValue<T>(instance, newValue);

                        if (!operation->IsEditorGui())
                        {
	                        target->OnFieldChanged(relativeFieldPath);
                        }
                    }

                    return false;
                },
                [targetRef, relativeFieldPath, initialValue](const Ref<EditorOperation>& operation)
                {
                    if (auto target = targetRef.Lock())
                    {
                        Ptr<FieldType> field;
                        void* instance = nullptr;
                        bool success = target->GetClass()->FindFieldInstanceRelative(relativeFieldPath, target,
                            field, instance);
                        if (!success)
                        {
                            return false;
                        }
                        field->SetFieldValue<T>(instance, initialValue);
                        target->OnFieldChanged(relativeFieldPath);
                    }

                    return false;
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
