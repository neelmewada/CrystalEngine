#include "FusionCore.h"

namespace CE
{

    FImage::FImage()
    {

    }

    void FImage::CalculateIntrinsicSize()
    {
        if (!m_Background.IsValidBrush())
        {
            Super::CalculateIntrinsicSize();
            ApplyIntrinsicSizeConstraints();
            return;
        }

        auto app = FusionApplication::Get();

        const CE::Name& imageName = m_Background.GetImageName();
        Vec2 brushSize = m_Background.GetBrushSize();

        if (imageName.IsValid())
        {
            auto image = app->GetImageAtlas()->FindImage(imageName);
            if (image.IsValid())
            {
                if (brushSize.x < 0)
                    brushSize.x = image.width;
                if (brushSize.y < 0)
                    brushSize.y = image.height;
            }
        }

        intrinsicSize = Vec2(Math::Max(brushSize.x, 0.0f), Math::Max(brushSize.y, 0.0f));
        ApplyIntrinsicSizeConstraints();
    }

    void FImage::Construct()
    {
        Super::Construct();


    }
    
}

