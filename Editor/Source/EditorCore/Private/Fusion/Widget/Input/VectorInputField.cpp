#include "EditorCore.h"

namespace CE::Editor
{

    VectorInputField::VectorInputField()
    {

    }

    void VectorInputField::Construct()
    {
        Super::Construct();

        (*this)
        .ContentVAlign(VAlign::Fill)
        .ContentHAlign(HAlign::Fill)
        (
            FAssignNew(NumericInputField, fieldX)
            .ColorTagVisible(true)
            .ColorTag(Color::Red())
            .VAlign(VAlign::Fill)
            .FillRatio(1.0f),

            FAssignNew(NumericInputField, fieldY)
            .ColorTagVisible(true)
            .ColorTag(Color::Green())
            .VAlign(VAlign::Fill)
            .FillRatio(1.0f),

            FAssignNew(NumericInputField, fieldZ)
            .ColorTagVisible(true)
            .ColorTag(Color::Blue())
            .VAlign(VAlign::Fill)
            .FillRatio(1.0f),

            FAssignNew(NumericInputField, fieldW)
            .ColorTagVisible(true)
            .ColorTag(Color::White())
            .VAlign(VAlign::Fill)
            .FillRatio(1.0f)
        );

        VectorType<Vec3>();
    }

    VectorInputField::Self& VectorInputField::VectorType(TypeId type)
    {
        thread_local HashSet<TypeId> floatVectors = { TYPEID(Vec2), TYPEID(Vec3), TYPEID(Vec4) };
        thread_local HashSet<TypeId> intVectors = { TYPEID(Vec2i), TYPEID(Vec3i), TYPEID(Vec4i) };

        if (floatVectors.Exists(type))
        {
            fieldX->NumericType<f32>();
            fieldY->NumericType<f32>();
            fieldZ->NumericType<f32>();
            fieldW->NumericType<f32>();
        }
        else if (intVectors.Exists(type))
        {
            fieldX->NumericType<s32>();
            fieldY->NumericType<s32>();
            fieldZ->NumericType<s32>();
            fieldW->NumericType<s32>();
        }
        else
        {
            return *this;
        }

        fieldX->Enabled(true);
        fieldY->Enabled(true);
        fieldZ->Enabled(false);
        fieldW->Enabled(false);

        if (type == TYPEID(Vec3) || type == TYPEID(Vec3i))
        {
            fieldZ->Enabled(true);
        }
        else if (type == TYPEID(Vec4) || type == TYPEID(Vec4i))
        {
            fieldZ->Enabled(true);
            fieldW->Enabled(true);
        }

        return *this;
    }
}

