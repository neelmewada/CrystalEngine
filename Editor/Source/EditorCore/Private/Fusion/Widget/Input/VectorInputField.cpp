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
    
}

