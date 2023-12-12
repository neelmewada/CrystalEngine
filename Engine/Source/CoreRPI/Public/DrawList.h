#pragma once

namespace CE::RPI
{
	using DrawListTag = Handle<u8>;
	using DrawListMask = BitSet<64>;

	using DrawList = Array<DrawItem>;
    
} // namespace CE::RPI
