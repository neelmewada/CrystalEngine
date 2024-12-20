#include "EditorCore.h"

namespace CE::Editor
{

    EditorHistory::EditorHistory()
    {
        historyStack.Reserve(OperationStackSize);
    }

    void EditorHistory::PerformOperation(const String& name, const Ref<Object>& target,
        const EditorOperationDelegate& execute,
	    const EditorOperationDelegate& unexecute)
    {
        Array targets = { target };
        PerformOperation(name, targets, execute, unexecute);
    }

    void EditorHistory::PerformOperation(const String& name, const Array<Ref<Object>>& targets,
	    const EditorOperationDelegate& execute,
        const EditorOperationDelegate& unexecute)
    {
        if (!execute.IsValid() || !unexecute.IsValid())
            return;

        String fixedName = name;

        if (!IsValidObjectName(fixedName))
        {
            fixedName = FixObjectName(fixedName);
        }

        Ref<EditorOperation> operation = CreateObject<EditorOperation>(this, fixedName);

        operation->history = this;

        operation->execute = execute;
        operation->unexecute = unexecute;

        for (const auto& target : targets)
        {
            operation->targets.Add(target);
        }

        execute.Invoke(operation);

        while (topIndex < historyStack.GetSize() - 1)
        {
            historyStack[topIndex - 1]->BeginDestroy();
            historyStack.RemoveAt(topIndex + 1);
        }

        historyStack.Add(operation);

        topIndex = historyStack.GetSize() - 1;
    }

    void EditorHistory::Undo()
    {
        if (topIndex >= 0)
        {
            historyStack[topIndex]->unexecute.Invoke(historyStack[topIndex]);
            topIndex--;
        }
    }

    void EditorHistory::Redo()
    {
        if (topIndex < historyStack.GetSize() - 1)
        {
            topIndex++;
            historyStack[topIndex]->execute.Invoke(historyStack[topIndex]);
        }
    }


}

