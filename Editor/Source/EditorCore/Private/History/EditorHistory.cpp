#include "EditorCore.h"

namespace CE::Editor
{

    EditorHistory::EditorHistory()
    {
        historyStack.Reserve(OperationStackSize);
    }

    void EditorHistory::PerformOperation(const String& name, const Ref<Object>& target,
                                         const EditorOperationDelegate& execute,
                                         const EditorOperationDelegate& unexecute,
                                         const SubClass<EditorOperation>& operationClass)
    {
        Array targets = { target };
        PerformOperation(name, targets, execute, unexecute, operationClass);
    }

    void EditorHistory::PerformOperation(const String& name, const Array<Ref<Object>>& targets,
	    const EditorOperationDelegate& execute,
        const EditorOperationDelegate& unexecute,
        const SubClass<EditorOperation>& operationClass)
    {
        if (!execute.IsValid() || !unexecute.IsValid())
            return;

        String fixedName = name;

        if (!IsValidObjectName(fixedName))
        {
            fixedName = FixObjectName(fixedName);
        }

        Ref<EditorOperation> operation = CreateObject<EditorOperation>(this, fixedName, OF_NoFlags, operationClass);

        operation->title = name;
        operation->history = this;

        operation->execute = execute;
        operation->unexecute = unexecute;

        for (const auto& target : targets)
        {
            operation->targets.Add(target);
        }

        operation->isEditorGui = true;
        operation->isFirstTime = true;

        execute.Invoke(operation);

    	operation->isEditorGui = false;
        operation->isFirstTime = false;

        while (topIndex < (int)historyStack.GetSize() - 1)
        {
            historyStack[topIndex + 1]->BeginDestroy();
            historyStack.RemoveAt(topIndex + 1);
        }

        historyStack.Add(operation);

        topIndex = historyStack.GetSize() - 1;
    }

    void EditorHistory::Undo()
    {
        bool success = false;

        while (!success && topIndex >= 0)
        {
            success = historyStack[topIndex]->unexecute.Invoke(historyStack[topIndex]);
            topIndex--;

            break; // FIXME: skip invalid operations
        }
    }

    void EditorHistory::Redo()
    {
        bool success = false;

        while (!success && topIndex < (int)historyStack.GetSize() - 1)
        {
            topIndex++;
            success = historyStack[topIndex]->execute.Invoke(historyStack[topIndex]);

            break; // FIXME: skip invalid operations
        }
    }


}

