#pragma once

namespace CE::RHI
{
	/// @brief DrawListTag is a unique Id. Each DrawItem in a DrawPacket is assigned a DrawListTag
	/// which determines the DrawList the DrawItem is associated with.
	using DrawListTag = Handle<u8>;

	using DrawListMask = BitSet<Limits::Pipeline::DrawListTagCount>;

	using DrawListTagRegistry = TagRegistry<DrawListTag::IndexType, Limits::Pipeline::DrawListTagCount>;

	using DrawListView = ArrayView<DrawItemProperties>;

	class CORERHI_API DrawList final
	{
	public:

		void Clear();

		void AddDrawItem(DrawItemProperties drawItemProperties);

		void Merge(const DrawList& other);

		u32 GetDrawItemCount() const { return drawItems.GetSize(); }
		
		const DrawItemProperties& GetDrawItem(u32 index) const { return drawItems[index]; }

	private:

		HashMap<RHI::PipelineState*, Array<DrawItemProperties>> drawItemsByPipeline{};
		Array<DrawItemProperties> drawItems{};
		DrawListTag listTag = DrawListTag::NullValue;

		friend class DrawListContext;
		friend class Scope;
	};

	typedef StaticArray<DrawList, Limits::Pipeline::DrawListTagCount> DrawListsByTag;
    
} // namespace CE::RPI
