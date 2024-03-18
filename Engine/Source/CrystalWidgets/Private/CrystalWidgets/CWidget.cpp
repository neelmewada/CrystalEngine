#include "CrystalWidgets.h"

namespace CE::Widgets
{

	CWidget::CWidget()
	{
		node = YGNodeNew();
		YGNodeSetContext(node, this);
	}

	CWidget::~CWidget()
	{
		YGNodeFree(node);
		node = nullptr;
	}
    
} // namespace CE::Widgets
