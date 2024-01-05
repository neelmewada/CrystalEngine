#include "CoreRHI.h"

namespace CE::RHI
{
	void DrawList::Clear()
	{
		drawItemsByPipeline.Clear();
		drawItems.Clear();
	}

	void DrawList::AddDrawItem(DrawItemProperties drawItemProperties)
	{
		drawItemsByPipeline[drawItemProperties.item->pipelineState].Add(drawItemProperties);
		drawItems.Add(drawItemProperties);
	}

	void DrawList::Merge(const DrawList& other)
	{
		drawItemsByPipeline.AddRange(other.drawItemsByPipeline);
		drawItems.AddRange(other.drawItems);
	}

} // namespace CE::RHI
