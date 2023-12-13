#pragma once

namespace CE::RHI
{

	class CORERHI_API DrawPacketBuilder final
	{
	public:

		DrawPacket* Build();

	private:

		FixedArray<RHI::ScissorState, Limits::Pipeline::MaxColorAttachmentCount> scissors{};
		u32 scissorCount = 0;
		FixedArray<RHI::ViewportState, Limits::Pipeline::MaxColorAttachmentCount> viewports{};
		u32 viewportCount = 0;

	};
    
} // namespace CE::RPI
