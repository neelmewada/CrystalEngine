#include "CrystalEditor.h"


namespace CE::Editor
{

    AssetItemWidget::AssetItemWidget()
    {
		isInteractable = true;
		SetText("");
		SetLabel("");
    }

    AssetItemWidget::~AssetItemWidget()
    {
        
    }

	void AssetItemWidget::SetAsFolder()
	{
		icon = GetStyleManager()->SearchImageResource("/Icons/folder.png");
		RemoveStyleClass("Asset");
		isAssetItem = false;
	}

	void AssetItemWidget::SetAsAsset()
	{
		icon = GetStyleManager()->SearchImageResource("/Icons/file.png");
		AddStyleClass("Asset");
		isAssetItem = true;
	}

	void AssetItemWidget::OnBeforeComputeStyle()
	{
		auto style = stylesheet->SelectStyle(this, CStateFlag::Default, CSubControl::Alternate);
		if (style.properties.KeyExists(CStylePropertyType::Background))
		{
			bottomFrameColor = style.properties[CStylePropertyType::Background].color;
		}
	}

	void AssetItemWidget::Construct()
	{
		Super::Construct();

		LoadStyleSheet("/CrystalEditor/Resources/Styles/AssetItemWidget.css");
	}

	void AssetItemWidget::OnDrawGUI()
    {
		auto borderColor = defaultStyleState.borderColor;
		defaultStyleState.borderColor = Color(); // Clear out border color, we will draw it on top of everything

		Super::OnDrawGUI();
		
		defaultStyleState.borderColor = borderColor;

		auto rect = GetComputedLayoutRect();
		auto padding = GetComputedLayoutPadding();
		auto size = GetComputedLayoutSize();
		f32 rectCenterX = (rect.min.x + rect.max.x) / 2;

		const Vec2 imagePadding = Vec2(10, 0);
		auto imageSize = size.x - imagePadding.x;
		Rect imageRect = Rect(rectCenterX - size.x / 2 + imagePadding.x / 2, rect.y + imagePadding.y,
			rectCenterX + size.x / 2 - imagePadding.x / 2, rect.y + imagePadding.y + imageSize);

		if (icon.IsValid())
		{
			GUI::Image(imageRect, icon.id, {});
		}

		if (IsAsset() && bottomFrameColor.a > 0)
		{
			Rect bottomRect = Rect(rect.left, imageRect.bottom, rect.right, rect.bottom);
			FillRect(bottomFrameColor, bottomRect, Vec4(0, 0, defaultStyleState.borderRadius.z, defaultStyleState.borderRadius.w));
		}

		if (defaultStyleState.borderThickness > 0.2f)
			DrawRect(defaultStyleState.borderColor, rect, defaultStyleState.borderThickness, defaultStyleState.borderRadius);

		Rect textRect = Rect(rect.left + 3, imageRect.bottom, rect.right - 3, rect.bottom - 3);
		GUI::Text(textRect, label, defaultStyleState);
    }

	void AssetItemWidget::HandleEvent(CEvent* event)
	{
		if (event->type == CEventType::MouseButtonDoubleClick)
		{
			CMouseEvent* mouseEvent = (CMouseEvent*)event;
			if (mouseEvent->mouseButton == 0)
			{
				emit OnItemDoubleClicked();
				event->HandleAndStopPropagation();
			}
		}

		Super::HandleEvent(event);
	}

	void AssetItemWidget::OnClicked()
	{
		Super::OnClicked();


	}
    
} // namespace CE

