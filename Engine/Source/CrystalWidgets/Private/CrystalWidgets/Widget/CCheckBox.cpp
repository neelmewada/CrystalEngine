#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CCheckBox::CCheckBox()
	{
		receiveMouseEvents = true;

		isChecked = false;
	}

	void CCheckBox::SetChecked(bool checked)
	{
		if (isChecked == checked)
			return;

		isChecked = checked;

		SetNeedsPaint();

		emit OnCheckChanged(this);
	}

	void CCheckBox::Construct()
	{
		Super::Construct();


	}

	void CCheckBox::OnPaint(CPaintEvent* paintEvent)
	{
		Super::OnPaint(paintEvent);

		CPainter* painter = paintEvent->painter;

		RPI::Texture* image = CApplication::Get()->LoadImage("/Engine/Assets/Icons/Check");

		Vec2 imagePadding = Vec2(3.5f, 3.5f);
		Rect rect = Rect::FromSize(GetComputedLayoutTopLeft() + imagePadding, GetComputedLayoutSize() - imagePadding * 2);

		if (image && isChecked)
		{
			CBrush brush = CBrush(computedStyle.GetForegroundColor());
			painter->SetBrush(brush);

			painter->DrawTexture(rect, image);
		}
	}

	void CCheckBox::HandleEvent(CEvent* event)
	{
		if (event->IsMouseEvent())
		{
			CMouseEvent* mouseEvent = static_cast<CMouseEvent*>(event);

			if (mouseEvent->type == CEventType::MouseRelease && mouseEvent->isInside && mouseEvent->button == MouseButton::Left)
			{
				SetChecked(!isChecked);
			}
		}

		Super::HandleEvent(event);
	}

} // namespace CE::Widgets
