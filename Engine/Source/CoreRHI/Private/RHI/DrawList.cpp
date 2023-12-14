#include "CoreRHI.h"

namespace CE::RHI
{
	void DrawList::Clear()
	{
		drawItemsByPipeline.Clear();
	}

	void DrawList::AddDrawItem(DrawItemProperties drawItemProperties)
	{
		drawItemsByPipeline[drawItemProperties.item->pipelineState].Add(drawItemProperties);
	}

	void DrawList::Merge(const DrawList& other)
	{
		drawItemsByPipeline.AddRange(other.drawItemsByPipeline);
	}

} // namespace CE::RHI
