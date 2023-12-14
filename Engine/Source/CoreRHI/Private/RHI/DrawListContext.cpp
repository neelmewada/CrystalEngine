#include "CoreRHI.h"

namespace CE::RHI
{

	DrawList& DrawListContext::GetDrawListForTag(u8 tag)
	{
		static DrawList empty{};
		if (tag >= mergedDrawListsByTag.GetSize())
			return empty;

		return mergedDrawListsByTag[tag];
	}

	void DrawListContext::AddDrawPacket(DrawPacket* drawPacket)
	{
		if (!drawPacket)
			return;

		DrawListsByTag& threadDrawListsByTag = this->threadDrawLists.GetStorage();

		for (int i = 0; i < drawPacket->GetDrawItemCount(); i++)
		{
			DrawItemProperties drawItemProperties = drawPacket->GetDrawItemProperties(i);
			DrawListTag drawListTag = drawPacket->drawListTags[i];

			auto& drawList = threadDrawListsByTag[drawListTag];
			drawList.AddDrawItem(drawItemProperties);
		}
	}

	void DrawListContext::Finalize()
	{
		for (int i = 0; i < mergedDrawListsByTag.GetSize(); i++)
		{
			mergedDrawListsByTag[i].Clear();
		}

		threadDrawLists.ForEach([&](DrawListsByTag& drawLists)
			{
				for (int i = 0; i < drawLists.GetSize(); i++)
				{
					mergedDrawListsByTag[i].Merge(drawLists[i]);
				}
			});
	}

} // namespace CE::RHI
