#include "CoreRHI.h"

namespace CE::RHI
{

    SwapChainFrameAttachment::SwapChainFrameAttachment(AttachmentID id, SwapChain* swapChain)
		: ImageFrameAttachment(id, swapChain->GetCurrentImage())
    {

    }

	void SwapChainFrameAttachment::UpdateImage()
	{
		SetResource(swapChain->GetCurrentImage());
	}

} // namespace CE::RHI
