#pragma once

namespace CE::Editor
{
    class EditorHistory;
    class EditorOperation;
    typedef Delegate<bool(const Ref<EditorOperation>&)> EditorOperationDelegate;

    CLASS()
    class EDITORCORE_API EditorOperation : public Object
    {
        CE_CLASS(EditorOperation, Object)
    protected:

        EditorOperation();

    public:

        void SetArrayIndex(int arrayIndex) { this->arrayIndex = arrayIndex; }
        int GetArrayIndex() const { return arrayIndex; }

        void SetEnumValue(s64 value) { this->enumValue = value; }
        s64 GetEnumValue() const { return enumValue; }

        bool IsEditorGui() const { return isEditorGui; }

    protected:

        int arrayIndex = -1;
        s64 enumValue = 0;
        bool isEditorGui = false;

        Variant userData;

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