#pragma once

namespace CE::RHI
{

	class CORERHI_API DrawListContext final
	{
	public:

		DrawList& GetDrawListForTag(u8 tag);

		void AddDrawPacket(DrawPacket* drawPacket);

		void Finalize();

	private:
		
		ThreadLocalContext<DrawListsByTag> threadDrawLists{};
		DrawListsByTag mergedDrawListsByTag{};
		DrawListMask drawListMask{};
	};
    
} // namespace CE::RHI
