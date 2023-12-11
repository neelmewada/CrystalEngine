#pragma once

namespace CE::RPI
{

	class CORERPI_API RenderTarget
	{
	public:
		RenderTarget(RHI::RenderTarget* rt);

		inline RHI::RenderTarget* GetRHIRenderTarget() const { return renderTarget; }

		inline bool IsNativeViewport() const { return renderTarget->IsViewportRenderTarget(); }

	private:

		RHI::RenderTarget* renderTarget = nullptr;
	};
    
} // namespace CE::RPI
