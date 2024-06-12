#include "FusionCore.h"

namespace CE
{

	FNativeContext::FNativeContext()
	{
		isIsolatedContext = true;
	}

	FNativeContext::~FNativeContext()
	{
		if (platformWindow)
		{
			
		}
	}

	FNativeContext* FNativeContext::Create(PlatformWindow* platformWindow, const String& name, FFusionContext* parentContext)
	{
		Object* outer = parentContext;
		if (outer == nullptr)
		{
			outer = FusionApplication::TryGet();
		}
		if (outer == nullptr)
		{
			return nullptr;
		}

		FNativeContext* nativeContext = CreateObject<FNativeContext>(outer, name);
		nativeContext->platformWindow = platformWindow;
		if (parentContext)
		{
			parentContext->AddChildContext(nativeContext);
		}
		nativeContext->Init();
		return nativeContext;
	}

	void FNativeContext::Init()
	{
		attachmentId = String::Format("Window_{}", platformWindow->GetWindowId());

		drawListTag = RPISystem::Get().GetDrawListTagRegistry()->AcquireTag(attachmentId);

		RHI::SwapChainDescriptor desc{};
		desc.imageCount = FrameScheduler::Get()->GetFramesInFlight();
		desc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };
		platformWindow->GetDrawableWindowSize(&desc.preferredWidth, &desc.preferredHeight);

		swapChain = RHI::gDynamicRHI->CreateSwapChain(platformWindow, desc);

		PlatformApplication::Get()->AddMessageHandler(this);

		renderer = CreateObject<FusionRenderer>(this, "FusionRenderer");

		UpdateViewConstants();

		FusionRendererInitInfo rendererInfo;
		rendererInfo.fusionShader = FusionApplication::Get()->GetFusionShader();
		rendererInfo.multisampling.sampleCount = 1;

		renderer->SetScreenSize(Vec2i(desc.preferredWidth, desc.preferredHeight));
		renderer->SetDrawListTag(drawListTag);

		renderer->Init(rendererInfo);

		FusionApplication::Get()->nativeWindows.Add(this);
	}

	void FNativeContext::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

		if (IsDefaultInstance())
			return;

		FusionApplication::Get()->nativeWindows.Remove(this);

		if (drawListTag.IsValid())
		{
			RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);
		}

		drawListTag = 0;

		PlatformApplication::Get()->RemoveMessageHandler(this);

		FusionApplication::Get()->QueueDestroy(renderer);
		renderer = nullptr;

		if (swapChain)
		{
			delete swapChain; swapChain = nullptr;
		}

		if (platformWindow)
		{
			PlatformApplication::Get()->DestroyWindow(platformWindow);
			platformWindow = nullptr;
		}
	}

	void FNativeContext::OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight)
	{
		ZoneScoped;

		if (!platformWindow || !renderer)
			return;
		if (platformWindow != window)
			return;

		if (swapChain)
		{
			swapChain->Rebuild();
		}

		UpdateViewConstants();

		MarkLayoutDirty();
	}

	void FNativeContext::OnWindowExposed(PlatformWindow* window)
	{
		ZoneScoped;

		if (!platformWindow || !renderer)
			return;
		if (platformWindow != window)
			return;

		if (swapChain)
		{
			swapChain->Rebuild();
		}

		UpdateViewConstants();

		MarkLayoutDirty();
	}

	Matrix4x4 OrthographicProjection(float left_plane,
	                                 float right_plane,
	                                 float bottom_plane,
	                                 float top_plane,
	                                 float near_plane,
	                                 float far_plane) {
		Matrix4x4 orthographic_projection_matrix = {
		  2.0f / (right_plane - left_plane),
		  0.0f,
		  0.0f,
		  0.0f,

		  0.0f,
		  2.0f / (bottom_plane - top_plane),
		  0.0f,
		  0.0f,

		  0.0f,
		  0.0f,
		  1.0f / (near_plane - far_plane),
		  0.0f,

		  -(right_plane + left_plane) / (right_plane - left_plane),
		  -(bottom_plane + top_plane) / (bottom_plane - top_plane),
		  near_plane / (near_plane - far_plane),
		  1.0f
		};
		return orthographic_projection_matrix;
	}

	void FNativeContext::UpdateViewConstants()
	{
		u32 screenWidth = 0; u32 screenHeight = 0;
		platformWindow->GetDrawableWindowSize(&screenWidth, &screenHeight);
		f32 aspectRatio = (f32)screenWidth / (f32)screenHeight;
		
		viewConstants.viewMatrix = Matrix4x4::Identity();
		viewConstants.projectionMatrix = Matrix4x4::Identity();

		Matrix4x4 rootTransform =
			Matrix4x4::Scale(Vec3(1.0f / screenWidth * 2, 1.0f / screenHeight * 2, 1)) *
			Matrix4x4::Translation(Vec3(-(f32)screenWidth * 0.5f, -(f32)screenHeight * 0.5f, 0));

		renderer->SetRootTransform(rootTransform);

		viewConstants.viewProjectionMatrix = viewConstants.projectionMatrix * viewConstants.viewMatrix;
		viewConstants.viewPosition = Vec4(0, 0, 0, 0);
		viewConstants.pixelResolution = Vec2(screenWidth, screenHeight);

		if (renderer)
		{
			renderer->SetViewConstants(viewConstants);
		}
	}

	void FNativeContext::Tick()
	{
		ZoneScoped;

		Super::Tick();

		// TODO: Rendering
		if (renderer && dirty)
		{
			renderer->Begin();

			renderer->End();

			dirty = false;
		}
	}

	void FNativeContext::EmplaceFrameAttachments()
	{
		Super::EmplaceFrameAttachments();

		FrameScheduler* scheduler = FrameScheduler::Get();

		FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();

		attachmentDatabase.EmplaceFrameAttachment(attachmentId, swapChain);
	}

	void FNativeContext::EnqueueScopes()
	{
		Super::EnqueueScopes();

		FrameScheduler* scheduler = FrameScheduler::Get();

		FrameAttachmentDatabase& attachmentDatabase = scheduler->GetAttachmentDatabase();

		if (platformWindow != nullptr && !platformWindow->IsMinimized() && platformWindow->IsShown())
		{
			scheduler->BeginScope(attachmentId);
			{
				RHI::ImageScopeAttachmentDescriptor swapChainAttachment{};
				swapChainAttachment.attachmentId = attachmentId;
				if (clearScreen)
				{
					swapChainAttachment.loadStoreAction.clearValue = Vec4(0, 0, 0, 1);
					swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Clear;
				}
				else
				{
					swapChainAttachment.loadStoreAction.loadAction = RHI::AttachmentLoadAction::Load;
				}
				
				swapChainAttachment.loadStoreAction.storeAction = RHI::AttachmentStoreAction::Store;
				swapChainAttachment.multisampleState.sampleCount = 1;
				scheduler->UseAttachment(swapChainAttachment, RHI::ScopeAttachmentUsage::Color, RHI::ScopeAttachmentAccess::ReadWrite);

				scheduler->PresentSwapChain(swapChain);
			}
			scheduler->EndScope();
		}
	}

	void FNativeContext::SetDrawListMask(RHI::DrawListMask& outMask)
	{
		Super::SetDrawListMask(outMask);

		if (drawListTag)
		{
			outMask.Set(drawListTag);
		}
	}

	void FNativeContext::EnqueueDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex)
	{
		Super::EnqueueDrawPackets(drawList, imageIndex);

		const auto& drawPackets = renderer->FlushDrawPackets(imageIndex);

		for (DrawPacket* drawPacket : drawPackets)
		{
			drawList.AddDrawPacket(drawPacket);
		}
	}

	void FNativeContext::SetDrawPackets(RHI::DrawListContext& drawList)
	{
		Super::SetDrawPackets(drawList);

		auto scheduler = FrameScheduler::Get();

		scheduler->SetScopeDrawList(attachmentId, &drawList.GetDrawListForTag(drawListTag));
	}

} // namespace CE
