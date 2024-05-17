#include "EditorSystem.h"

namespace CE::Editor
{
	Vec2 EditorSplitter::CalculateIntrinsicSize(f32 width, f32 height)
	{
		Vec2 size = Super::CalculateIntrinsicSize(width, height);

		return size;
	}

	bool EditorSplitter::IsLayoutCalculationRoot()
	{
		return false;
	}

	Vec2 EditorSplitter::GetAvailableSizeForChild(CWidget* childWidget)
	{
		return Super::GetAvailableSizeForChild(childWidget);
	}

	void EditorSplitter::SetOrientation(COrientation orientation)
	{
		this->orientation = orientation;

		SetNeedsStyle();
		SetNeedsLayout();
		SetNeedsPaint();
	}

    void EditorSplitter::Construct()
    {
        Super::Construct();


    }

    
} // namespace CE::Editor
