#include "CrystalEditor.h"

static const CE::String css = R"(
AssetItemWidget {
	height: 120px;
	width: 100px;
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

namespace CE::Widgets
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

		SetStyleSheet(css);

		
	}

	void AssetItemWidget::OnDrawGUI()
    {
		Super::OnDrawGUI();
    }

	void AssetItemWidget::HandleEvent(CEvent* event)
	{
		

		Super::HandleEvent(event);
	}
    
} // namespace CE

