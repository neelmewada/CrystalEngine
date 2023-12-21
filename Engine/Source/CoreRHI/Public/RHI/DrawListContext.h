#pragma once

namespace CE::RHI
{

	class CORERHI_API DrawListContext final
	{
	public:

		void Init(const DrawListMask& drawListMask);
		void Shutdown();

		DrawList& GetDrawListForTag(u8 tag);

		/// @brief Thread safe function to add a draw package to the DrawList.
		/// @param drawPacket to add.
		void AddDrawPacket(DrawPacket* drawPacket);

		void Finalize();

	private:
		
		ThreadLocalContext<DrawListsByTag> threadDrawListsByTag{};
		DrawListsByTag mergedDrawListsByTag{};
		DrawListMask drawListMask{};
	};
    
} // namespace CE::RHI
