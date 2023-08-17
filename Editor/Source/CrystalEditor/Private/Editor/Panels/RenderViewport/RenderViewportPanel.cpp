#include "CrystalEditor.h"

namespace CE::Editor
{
	RenderViewportPanel::RenderViewportPanel()
	{
		defaultDockPosition = CDockPosition::Center;
	}

	RenderViewportPanel::~RenderViewportPanel()
	{
		if (gfxCommandList != nullptr)
		{
			RHI::gDynamicRHI->DestroyCommandList(gfxCommandList);
			gfxCommandList = nullptr;
		}

		if (viewportRT != nullptr)
		{
			RHI::gDynamicRHI->DestroyRenderTarget(viewportRT);
			viewportRT = nullptr;
		}
	}

	void RenderViewportPanel::Construct()
	{
		Super::Construct();

		SetTitle("Viewport");

		if (gfxCommandList == nullptr)
		{
			RHI::RenderTargetLayout layout{};
			layout.backBufferCount = 1;
			layout.numColorOutputs = 1;
			layout.colorOutputs[0].preferredFormat = RHI::ColorFormat::Auto;
			layout.depthStencilFormat = RHI::DepthStencilFormat::Auto;
			layout.simultaneousFrameDraws = 1;
			layout.presentationRTIndex = -1;
			
			viewportRT = RHI::gDynamicRHI->CreateRenderTarget(512, 512, layout);

			gfxCommandList = RHI::gDynamicRHI->CreateGraphicsCommandList(viewportRT);
		}
	}

	void RenderViewportPanel::OnDrawGUI()
	{
		Super::OnDrawGUI();

		

	}

} // namespace CE::Editor
