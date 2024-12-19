#include "EditorCore.h"

namespace CE::Editor
{

    EditorHistory::EditorHistory()
    {
        
    }

    void EditorHistory::PerformOperation(const String& name, const Ref<Object>& target,
        const EditorOperationDelegate& execute,
	    const EditorOperationDelegate& unexecute)
    {
        Array<Ref<Object>> targets = { target };
        PerformOperation(name, targets, execute, unexecute);
    }

    void EditorHistory::PerformOperation(const String& name, const Array<Ref<Object>>& targets,
	    const EditorOperationDelegate& execute,
        const EditorOperationDelegate& unexecute)
    {
        if (!execute.IsValid() || !unexecute.IsValid())
            return;

        Ref<EditorOperation> operation = CreateObject<EditorOperation>(this, name);

        operation->execute = execute;
        operation->unexecute = unexecute;

        execute.Invoke(operation);

        historyStack.Add(operation);
    }

}

