#include "CrystalEditor.h"

namespace CE::Editor
{
	RenderViewportPanel::RenderViewportPanel()
	{
		defaultDockPosition = CDockPosition::Center;
	}

	RenderViewportPanel::~RenderViewportPanel()
	{
		if (guiTexture != nullptr)
		{
			RHI::gDynamicRHI->RemoveImGuiTexture(guiTexture);
			guiTexture = nullptr;
		}

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
			layout.colorOutputs[0].preferredFormat = RHI::ColorFormat::RGBA32;
			layout.colorOutputs[0].sampleCount = 1;
			layout.colorOutputs[0].loadAction = RHI::RenderPassLoadAction::Clear;
			layout.colorOutputs[0].storeAction = RHI::RenderPassStoreAction::Store;
			layout.depthStencilFormat = RHI::DepthStencilFormat::None; // TODO: enable depth texture
			layout.simultaneousFrameDraws = 1;
			layout.presentationRTIndex = -1;
			
			viewportRT = RHI::gDynamicRHI->CreateRenderTarget(currentResolution.x, currentResolution.y, layout);
			viewportRT->SetClearColor(0, Color::FromRGBA32(34, 59, 99, 255));
			
			gfxCommandList = RHI::gDynamicRHI->CreateGraphicsCommandList(viewportRT);
		}
	}

	void RenderViewportPanel::OnDrawGUI()
	{
		auto rect = GetComputedLayoutRect();
		auto winSize = GetWindowSize();
		bool shouldShowImage = false;

		if (!isnan(winSize.x) && !isnan(winSize.y) && winSize.x > 0 && winSize.y > 0)
		{
			shouldShowImage = true;
			if (winSize.x != currentResolution.x || winSize.y != currentResolution.y)
			{
				currentResolution = Vec2i(winSize.x, winSize.y);
				viewportRT->Resize(currentResolution);

				auto texture = viewportRT->GetColorTargetTexture(0);
				auto sampler = viewportRT->GetColorTargetTextureSampler(0);

				guiTexture = RHI::gDynamicRHI->AddImGuiTexture(texture, sampler);
			}
		}

		if (shouldShowImage && guiTexture != nullptr)
		{
			gfxCommandList->Begin();

			gfxCommandList->End();

			RHI::gDynamicRHI->ExecuteCommandList(gfxCommandList);

			SetBackgroundImage(guiTexture);
		}
		else
		{
			SetBackgroundImage(nullptr);
		}

		Super::OnDrawGUI();
	}

} // namespace CE::Editor
