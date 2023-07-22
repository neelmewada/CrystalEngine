#include "CoreWidgets.h"

namespace CE::Widgets
{

    CSeparator::CSeparator()
    {
		stateFlags = CStateFlag::Vertical;
    }

    CSeparator::~CSeparator()
    {
        
    }

	Vec2 CSeparator::CalculateIntrinsicContentSize(f32 width, f32 height)
	{
		Vec2 size = Vec2();
		if (stateFlags == CStateFlag::Horizontal)
		{
			if (height != 0 && !isnan(height))
				size.y = height;
			size.x = 2;
		}
		else // Vertical
		{
			if (width != 0 && !isnan(width))
				size.x = width;
			size.y = 2;
		}
		return size;
	}

	void CSeparator::OnAfterComputeStyle()
	{
		Super::OnAfterComputeStyle();

		auto parent = GetOwner();

		if (parent != nullptr)
		{
			auto node = parent->GetNode();

			auto flexDir = YGNodeStyleGetFlexDirection(node);
			
			if (flexDir == YGFlexDirectionColumn || flexDir == YGFlexDirectionColumnReverse)
			{
				stateFlags = CStateFlag::Vertical;
			}
			else if (flexDir == YGFlexDirectionRow || flexDir == YGFlexDirectionRowReverse)
			{
				stateFlags = CStateFlag::Horizontal;
			}
		}
	}

    void CSeparator::OnDrawGUI()
    {
		DrawDefaultBackground();
    }
    
} // namespace CE

