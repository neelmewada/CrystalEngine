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
		nativeContext->Init();

		if (parentContext)
		{
			parentContext->AddChildContext(nativeContext);
		}
		return nativeContext;
	}

	void FNativeContext::Init()
	{
		attachmentId = String::Format("Window_{}", platformWindow->GetWindowId());

		RHI::SwapChainDescriptor desc{};
		desc.imageCount = FrameScheduler::Get()->GetFramesInFlight();
		desc.preferredFormats = { RHI::Format::R8G8B8A8_UNORM, RHI::Format::B8G8R8A8_UNORM };
		platformWindow->GetDrawableWindowSize(&desc.preferredWidth, &desc.preferredHeight);

		swapChain = RHI::gDynamicRHI->CreateSwapChain(platformWindow, desc);

		PlatformApplication::Get()->AddMessageHandler(this);

		renderer = CreateObject<FusionRenderer>(this, "FusionRenderer");

		UpdateViewConstants();
	}

	void FNativeContext::OnBeforeDestroy()
	{
		Super::OnBeforeDestroy();

		if (IsDefaultInstance())
			return;

		PlatformApplication::Get()->RemoveMessageHandler(this);

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
	}

	void FNativeContext::UpdateViewConstants()
	{
		u32 screenWidth = 0; u32 screenHeight = 0;
		platformWindow->GetDrawableWindowSize(&screenWidth, &screenHeight);

		viewConstants.viewMatrix = Matrix4x4::Identity();
		viewConstants.projectionMatrix = Matrix4x4::Translation(Vec3(-1, -1, 0)) *
			Matrix4x4::Scale(Vec3(1.0f / screenWidth, 1.0f / screenHeight, 1)) *
			Quat::EulerDegrees(0, 0, 0).ToMatrix();

		viewConstants.viewProjectionMatrix = viewConstants.projectionMatrix * viewConstants.viewMatrix;
		viewConstants.viewPosition = Vec4(0, 0, 0, 0);
		viewConstants.pixelResolution = Vec2(screenWidth, screenHeight);

		
	}

	void FNativeContext::Tick()
	{
		ZoneScoped;

		Super::Tick();

		// TODO: Rendering
	}

	void FNativeContext::EmplaceFrameAttachments(FrameAttachmentDatabase& attachmentDatabase)
	{
		Super::EmplaceFrameAttachments(attachmentDatabase);

		attachmentDatabase.EmplaceFrameAttachment(attachmentId, swapChain);
	}

} // namespace CE
