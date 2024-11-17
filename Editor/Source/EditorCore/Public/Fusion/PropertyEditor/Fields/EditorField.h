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
        Self& BindField(FieldType* field, Object* target);

        virtual Self& UnbindField();

        bool IsBound() const { return field != nullptr && targets.NonEmpty(); }

        virtual EditorField& FixedInputWidth(f32 width);

        virtual void UpdateValue() = 0;

    protected: // - Internal -

        FieldType* field = nullptr;
        Array<Object*> targets;
        Array<void*> instances;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
        friend class PropertyEditor;
        friend class ArrayPropertyEditor;
    };
    
}

#include "EditorField.rtti.h"
