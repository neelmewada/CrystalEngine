#include "CoreRHI.h"

namespace CE::RHI
{

    SwapChainFrameAttachment::SwapChainFrameAttachment(AttachmentID id, SwapChain* swapChain)
		: ImageFrameAttachment(id, swapChain->GetCurrentImage())
		, swapChain(swapChain)
    {

    }

	void SwapChainFrameAttachment::UpdateImage()
	{
		SetResource(swapChain->GetCurrentImage());
	}

} // namespace CE::RHI
