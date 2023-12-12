#pragma once

namespace CE::RPI
{

	class CORERPI_API DrawPacket final : public IntrusiveBase
	{
	public:

		u8 drawItemCount = 0;

		DrawItem* drawItems = nullptr;

	private:

		friend class DrawPacketBuilder;
	};
    
} // namespace CE::RPI
