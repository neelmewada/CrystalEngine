#include "CoreRHI.h"

namespace CE::RHI
{

    SwapChainFrameAttachment::SwapChainFrameAttachment(AttachmentID id, SwapChain* swapChain)
		: ImageFrameAttachment(id, nullptr)
		, swapChain(swapChain)
    {
		for (int i = 0; i < swapChain->GetImageCount(); i++)
		{
			SetResource(i, swapChain->GetImage(i));
		}
    }

	void SwapChainFrameAttachment::UpdateImage()
	{
		
	}

} // namespace CE::RHI
