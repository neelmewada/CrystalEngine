#include "FusionCore.h"

namespace CE
{

    FImageButton::FImageButton()
    {

    }

    void FImageButton::Construct()
    {
        Super::Construct();

        Child(
            FAssignNew(FImage, image)
            
        );
    }

    FImageButton::Self& FImageButton::ImageWidth(f32 width)
    {
        image->Width(width);
        return *this;
    }

    FImageButton::Self& FImageButton::ImageHeight(f32 height)
    {
        image->Height(height);
        return *this;
    }

}

