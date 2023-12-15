#pragma once

namespace CE::RHI
{
	/// @brief DrawListTag is a unique Id. Each DrawItem in a DrawPacket is assigned a DrawListTag
	/// which determines the DrawList the DrawItem is associated with.
	using DrawListTag = Handle<u8>;

	using DrawListMask = BitSet<Limits::Pipeline::DrawListTagCount>;

	class CORERHI_API DrawList final
	{
	public:

		void Clear();

		void AddDrawItem(DrawItemProperties drawItemProperties);

		void Merge(const DrawList& other);

	private:

		DrawListTag listTag = DrawListTag::NullValue;

		HashMap<RHI::IPipelineState*, Array<DrawItemProperties>> drawItemsByPipeline{};

		friend class DrawListContext;
	};

	typedef FixedArray<DrawList, Limits::Pipeline::DrawListTagCount> DrawListsByTag;
    
} // namespace CE::RPI
