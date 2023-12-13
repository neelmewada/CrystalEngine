#pragma once

namespace CE::RHI
{

	class CORERHI_API DrawListContext final
	{
	public:

		DrawList& GetDrawListForTag(u8 tag);

	private:

		
		DrawListsByTag drawListsByTag{};
		DrawListMask drawListMask{};
	};
    
} // namespace CE::RHI
