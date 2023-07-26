#include "CrystalEditor.h"

static const CE::String css = R"(
AssetItemWidget {
	background: rgb(26, 26, 26);
	border-radius: 5px;
	border-width: 2px;
	border-color: rgba(0, 0, 0, 0);
}

AssetItemWidget:hovered {
	border-color: rgba(0, 112, 224, 255);
}

AssetItemWidget:pressed {
	border-color: rgba(0, 112, 224, 120);
}

)";

static const CE::String mediumSize = R"(
AssetItemWidget {
    height: 120px;
    width: 100px;
}
)";

namespace CE::Editor
{

    AssetItemWidget::AssetItemWidget()
    {
		isInteractable = true;
		SetText("");
    }

    AssetItemWidget::~AssetItemWidget()
    {
        
    }

	void AssetItemWidget::Construct()
	{
		Super::Construct();

		SetStyleSheet(css + mediumSize);
		
	}

	void AssetItemWidget::OnDrawGUI()
    {
		Super::OnDrawGUI();

		auto rect = GetComputedLayoutRect();
		auto size = GetComputedLayoutSize();
		f32 rectCenterX = (rect.min.x + rect.max.x) / 2;

		if (!icon.IsValid())
			icon = GetStyleManager()->SearchImageResource("/Icons/folder.png");

		if (icon.IsValid())
		{
			const Vec2 padding = Vec2(20, 0);
			auto imageSize = size.x - padding.x;
			Rect imageRect = Rect(rectCenterX - size.x / 2 + padding.x / 2, rect.y + padding.y,
				rectCenterX + size.x / 2 - padding.x / 2, rect.y + padding.y + imageSize);

			GUI::Image(imageRect, icon.id, {});
		}
    }

	void AssetItemWidget::HandleEvent(CEvent* event)
	{
		

		Super::HandleEvent(event);
	}
    
} // namespace CE

