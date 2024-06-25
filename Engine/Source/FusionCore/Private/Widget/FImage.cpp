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

        const CE::Name& texturePath = m_Background.GetTexturePath();
        Vec2 brushSize = m_Background.GetBrushSize();
        
        intrinsicSize = brushSize;
        ApplyIntrinsicSizeConstraints();
    }

    void FImage::Construct()
    {
        Super::Construct();


    }
    
}

