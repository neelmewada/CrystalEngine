#pragma once

namespace CE::RPI
{

	class CORERPI_API RenderViewport
	{
	public:
		RenderViewport(RHI::RenderTarget* rt);

		inline RHI::RenderTarget* GetRenderTarget() const { return renderTarget; }

		inline bool IsNativeViewport() const { return renderTarget->IsViewportRenderTarget(); }

	private:

		RHI::RenderTarget* renderTarget = nullptr;
	};
    
} // namespace CE::RPI
