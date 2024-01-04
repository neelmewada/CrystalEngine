#pragma once

namespace CE::RHI
{
    
	class CORERHI_API SwapChainFrameAttachment : public ImageFrameAttachment
	{
	public:

		SwapChainFrameAttachment(AttachmentID id, SwapChain* swapChain);

		inline SwapChain* GetSwapChain() const
		{
			return swapChain;
		}

		void UpdateImage();

	private:

		SwapChain* swapChain = nullptr;
	};

} // namespace CE::RHI
