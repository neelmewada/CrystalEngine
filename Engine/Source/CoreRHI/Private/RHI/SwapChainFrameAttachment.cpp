#include "CoreRHI.h"

namespace CE::RHI
{

    SwapChainFrameAttachment::SwapChainFrameAttachment(AttachmentID id, SwapChain* swapChain)
		: ImageFrameAttachment(id, nullptr)
		, swapChain(swapChain)
    {
		for (int i = 0; i < swapChain->GetImageCount(); i++)
		{
			Texture* image = swapChain->GetImage(i);
			if (image)
			{
				descriptor.width = image->GetWidth();
				descriptor.height = image->GetHeight();
				descriptor.depth = image->GetDepth();
				descriptor.format = image->GetFormat();
				descriptor.sampleCount = image->GetSampleCount();
				descriptor.dimension = image->GetDimension();
				descriptor.arrayLayers = image->GetArrayLayerCount();
				descriptor.bindFlags = image->GetBindFlags();
				descriptor.mipLevels = image->GetMipLevelCount();
				descriptor.defaultHeapType = MemoryHeapType::Default;
			}

			SetResource(i, image);
		}
    }

	void SwapChainFrameAttachment::UpdateImage()
	{
		
	}

} // namespace CE::RHI
