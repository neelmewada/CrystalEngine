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

		//GUI::PushChildCoordinateSpace(rect);

		//if (icon.IsValid())
		{
			//GUI::Image(Rect(300, 300, 400, 400), icon.id, {});
		}

		//GUI::PopChildCoordinateSpace();
    }

	void AssetItemWidget::HandleEvent(CEvent* event)
	{
		

		Super::HandleEvent(event);
	}
    
} // namespace CE

