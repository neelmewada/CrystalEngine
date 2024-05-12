#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CIcon::CIcon()
    {

    }

    CIcon::~CIcon()
    {
	    
    }

    Vec2 CIcon::CalculateIntrinsicSize(f32 width, f32 height)
    {
        RPI::Texture* image = nullptr;

        if (backgroundImageOverride.IsValid())
        {
            image = CApplication::Get()->LoadImage(backgroundImageOverride);
        }

        if (image == nullptr && computedStyle.properties.KeyExists(CStylePropertyType::BackgroundImage))
        {
            Name imageName = computedStyle.properties[CStylePropertyType::BackgroundImage].string;

            if (imageName.IsValid())
            {
                image = CApplication::Get()->LoadImage(imageName);
            }
        }

        if (!image)
            return Vec2();

        return Vec2(image->GetWidth(), image->GetHeight());
    }

    void CIcon::Construct()
    {
	    Super::Construct();

        
    }

    void CIcon::OnPaint(CPaintEvent* paintEvent)
    {
	    Super::OnPaint(paintEvent);
    }

} // namespace CE::Widgets
