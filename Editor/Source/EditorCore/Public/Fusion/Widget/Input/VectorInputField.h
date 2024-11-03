#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API VectorInputField : public FHorizontalStack
    {
        CE_CLASS(VectorInputField, FHorizontalStack)
    protected:

        VectorInputField();

        void Construct() override;

    public: // - Public API -


    protected: // - Internal -

        NumericInputField* fieldX = nullptr;
        NumericInputField* fieldY = nullptr;
        NumericInputField* fieldZ = nullptr;
        NumericInputField* fieldW = nullptr;

    public: // - Fusion Properties - 

        template<typename T> requires TContainsType<T, Vec2, Vec3, Vec4, Vec2i, Vec3i, Vec4i>::Value
        Self& VectorType()
        {
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

        FUSION_WIDGET;
    };
    
}

#include "VectorInputField.rtti.h"
