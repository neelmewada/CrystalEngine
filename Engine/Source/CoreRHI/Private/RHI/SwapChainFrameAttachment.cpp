#include "CoreRHI.h"

namespace CE::RHI
{

    SwapChainFrameAttachment::SwapChainFrameAttachment(AttachmentID id, SwapChain* swapChain)
		: ImageFrameAttachment(id, nullptr)
		, swapChain(swapChain)
    {
		for (int i = 0; i < resources.GetSize(); i++)
		{
			SetResource(swapChain->GetImage(i));
		}
    }

	void SwapChainFrameAttachment::UpdateImage()
	{
		
	}

} // namespace CE::RHI
