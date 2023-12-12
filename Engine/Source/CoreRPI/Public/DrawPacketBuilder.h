#pragma once

namespace CE::RPI
{

	class CORERPI_API DrawPacketBuilder final
	{
	public:

		DrawPacket* Build();

	private:

		FixedArray<RHI::ScissorRect, Limits::Pipeline::MaxColorAttachmentCount> scissors{};
		u32 scissorCount = 0;
		FixedArray<RHI::ViewportRect, Limits::Pipeline::MaxColorAttachmentCount> viewports{};
		u32 viewportCount = 0;

	};
    
} // namespace CE::RPI
