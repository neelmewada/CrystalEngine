#include "CrystalWidgets.h"

namespace CE::Widgets
{

    CImage::CImage()
    {
        
    }

    CImage::~CImage()
    {
	    
    }

    Vec2 CImage::CalculateIntrinsicSize(f32 width, f32 height)
    {
        RPI::Texture* image = nullptr;

        if (backgroundImageOverride.IsValid())
        {
            image = CApplication::Get()->LoadImage(backgroundImageOverride);
        }
        else if (computedStyle.properties.KeyExists(CStylePropertyType::BackgroundImage))
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

    void CImage::Construct()
    {
	    Super::Construct();

        
    }

    void CImage::HandleEvent(CEvent* event)
    {
        if (event->IsMouseEvent())
        {
            CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

            if (mouseEvent->type == CEventType::MousePress && mouseEvent->button == MouseButton::Left)
            {
                emit OnMouseLeftPress();
            }
            else if (mouseEvent->type == CEventType::MouseRelease && mouseEvent->button == MouseButton::Left && mouseEvent->isInside)
            {
                emit OnMouseLeftClick();
            }
        }

	    Super::HandleEvent(event);
    }

    void CImage::OnPaint(CPaintEvent* paintEvent)
    {
	    Super::OnPaint(paintEvent);
    }

} // namespace CE::Widgets
