#pragma once

namespace CE::RPI
{
	/// @brief DrawListTag is a unique Id. Each DrawItem in a DrawPacket is assigned a DrawListTag
	/// which determines the DrawList the DrawItem is target of.
	using DrawListTag = Handle<u8>;

	using DrawListMask = BitSet<Limits::Pipeline::DrawListTagCount>;

	using DrawList = Array<DrawItemProperties>;
    
} // namespace CE::RPI
