#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class EDITORCORE_API EditorField : public FCompoundWidget
    {
        CE_CLASS(EditorField, FCompoundWidget)
    protected:

        EditorField();

        void Construct() override;

        void OnBeginDestroy() override;

    public: // - Public API -

        virtual bool CanBind(FieldType* field) = 0;

        virtual Self& BindField(FieldType* field, Object* target, void* instance);
        virtual Self& BindField(const Ref<Object>& target, const CE::Name& relativeFieldPath);

        Self& BindField(FieldType* field, Object* target);

        virtual Self& UnbindField();

        bool IsBound() const { return field != nullptr && targets.NotEmpty() && instances.NotEmpty(); }

        virtual EditorField& FixedInputWidth(f32 width);

        virtual void OnBind() {}

        virtual void UpdateValue() = 0;

    protected: // - Internal -

        bool isBound = false;
        Array<WeakRef<Object>> targets;
        FieldType* field = nullptr;
        CE::Name relativeFieldPath;

        // TODO: We should NOT be storing raw void pointer to instances.
        // Make a way to fetch relative instance from field and target Object every time it's needed.
        Array<void*> instances;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(WeakRef<EditorHistory>, History);

        FUSION_WIDGET;
        friend class PropertyEditor;
        friend class ArrayPropertyEditor;
    };
    
}

#include "EditorField.rtti.h"
