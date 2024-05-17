#include "EditorSystem.h"

namespace CE::Editor
{
	Vec2 EditorSplitterContainer::CalculateIntrinsicSize(f32 width, f32 height)
	{
		Vec2 size = Super::CalculateIntrinsicSize(width, height);
		return size;
	}

	bool EditorSplitterContainer::IsLayoutCalculationRoot()
	{
        return true;
	}

    void EditorSplitterContainer::Construct()
    {
        Super::Construct();


    }

    
} // namespace CE::Editor
