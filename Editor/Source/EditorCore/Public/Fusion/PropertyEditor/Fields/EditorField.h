#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class EDITORCORE_API EditorField : public FCompoundWidget, IObjectUpdateListener
    {
        CE_CLASS(EditorField, FCompoundWidget)
    protected:

        EditorField();

        void Construct() override;

    public: // - Public API -

        virtual bool CanBind(FieldType* field) = 0;

        virtual Self& BindField(FieldType* field, Object* target);

        virtual Self& UnbindField();

        bool IsBound() const { return field != nullptr && targets.NonEmpty(); }

    protected: // - Internal -

        void OnObjectFieldChanged(Object* object, const CE::Name& fieldName) override final;

        virtual void UpdateValue() = 0;

        FieldType* field = nullptr;
        Array<Object*> targets;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "EditorField.rtti.h"
