#pragma once

namespace CE::RPI
{

	/// @brief DrawPacket is a collection of DrawItems that represent a single mesh. Each DrawItem represent a single pass on the mesh
	/// and is associated with a DrawListTag which denotes the DrawList the DrawItem is associated to. DrawListTag can be used to
	/// determine the pass for ex: Shadow Pass, Forward+ Pass, Transparent pass, etc.
	class CORERPI_API DrawPacket final : public IntrusiveBase
	{
	public:

		u8 drawItemCount = 0;

		/// @brief List of draw items. Count = drawItemCount.
		DrawItem* drawItems = nullptr;

		/// @brief List of draw list tags associated with the draw item index.
		DrawListTag* drawListTags = nullptr;

		/// @brief List of draw filter masks associated with the draw item index.
		DrawFilterMask* drawFilterMasks = nullptr;

	private:

		friend class DrawPacketBuilder;
	};
    
} // namespace CE::RPI
