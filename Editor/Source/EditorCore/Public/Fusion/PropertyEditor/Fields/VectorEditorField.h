#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API VectorEditorField : public EditorField
    {
        CE_CLASS(VectorEditorField, EditorField)
    protected:

        VectorEditorField();

        void Construct() override;

    public: // - Public API -


    protected: // - Internal -

        bool CanBind(FieldType* field) override;

        void UpdateValue() override;

        FUNCTION()
        void OnTextEdited(NumericEditorField* input);

        FUNCTION()
        void OnTextEditingFinished(NumericEditorField* input);

        FUNCTION()
        void OnEndEditing(NumericEditorField* input);

        FUNCTION()
        void OnBeginEditing(NumericEditorField* input);

        void SetFieldValue();

        NumericEditorField* fieldX = nullptr;
        NumericEditorField* fieldY = nullptr;
        NumericEditorField* fieldZ = nullptr;
        NumericEditorField* fieldW = nullptr;
        TypeId vectorTypeId = 0;
        Vec4 initialValue;

    public: // - Fusion Properties - 

        template<typename T> requires TContainsType<T, Vec2, Vec3, Vec4, Vec2i, Vec3i, Vec4i>::Value
        Self& VectorType()
        {
            vectorTypeId = TYPEID(T);

            fieldX->Enabled(true);
            fieldY->Enabled(true);
            fieldZ->Enabled(false);
            fieldW->Enabled(false);
            if constexpr (TContainsType<T, Vec2>::Value)
            {
                fieldX->NumericType<f32>();
                fieldY->NumericType<f32>();
                fieldZ->NumericType<f32>();
                fieldW->NumericType<f32>();
            }
            else if constexpr (TContainsType<T, Vec2i>::Value)
            {
                fieldX->NumericType<s32>();
                fieldY->NumericType<s32>();
                fieldZ->NumericType<s32>();
                fieldW->NumericType<s32>();
            }
            else if constexpr (TContainsType<T, Vec3>::Value)
            {
                fieldZ->Enabled(true);

                fieldX->NumericType<f32>();
                fieldY->NumericType<f32>();
                fieldZ->NumericType<f32>();
                fieldW->NumericType<f32>();
            }
            else if constexpr (TContainsType<T, Vec3i>::Value)
            {
                fieldZ->Enabled(true);

                fieldX->NumericType<s32>();
                fieldY->NumericType<s32>();
                fieldZ->NumericType<s32>();
                fieldW->NumericType<s32>();
            }
            else if constexpr (TContainsType<T, Vec4>::Value)
            {
                fieldZ->Enabled(true);
                fieldW->Enabled(true);

                fieldX->NumericType<f32>();
                fieldY->NumericType<f32>();
                fieldZ->NumericType<f32>();
                fieldW->NumericType<f32>();
            }
            else if constexpr (TContainsType<T, Vec4i>::Value)
            {
                fieldZ->Enabled(true);
                fieldW->Enabled(true);

                fieldX->NumericType<s32>();
                fieldY->NumericType<s32>();
                fieldZ->NumericType<s32>();
                fieldW->NumericType<s32>();
            }
            return *this;
        }

        Self& VectorType(TypeId type);

        FUSION_WIDGET;
    };
    
}

#include "VectorEditorField.rtti.h"
