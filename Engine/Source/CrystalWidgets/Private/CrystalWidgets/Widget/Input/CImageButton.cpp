#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CImageButton::CImageButton()
	{
		receiveDragEvents = false;
		receiveMouseEvents = true;
		allowVerticalScroll = allowHorizontalScroll = false;
		clipChildren = true;
	}

	void CImageButton::SetText(const String& text)
	{
		label->SetText(text);

		label->SetEnabled(!text.IsEmpty());
	}

	void CImageButton::SetImage(Name imagePath)
	{
		image->SetBackgroundImage(imagePath);
	}

	void CImageButton::Construct()
	{
		Super::Construct();

		image = CreateObject<CImage>(this, "Image");

		label = CreateObject<CLabel>(this, "Label");
		label->SetEnabled(false);
	}


} // namespace CE::Widgets
