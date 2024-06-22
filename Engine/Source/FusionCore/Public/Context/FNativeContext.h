#pragma once

namespace CE
{
	class FWindow;
	class FLayoutManager;
	class FWidget;
	class FusionRenderer;
	class FFusionContext;
	class FPainter;
	class FStyleSet;

	//! @brief Represents a native platform window.
	CLASS()
	class FUSIONCORE_API FNativeContext final : public FFusionContext, public ApplicationMessageHandler
	{
		CE_CLASS(FNativeContext, FFusionContext)
	public:

		FNativeContext();

		virtual ~FNativeContext();

		static FNativeContext* Create(PlatformWindow* platformWindow, const String& name, FFusionContext* parentContext = nullptr);

		PlatformWindow* GetPlatformWindow() const { return platformWindow; }

		void Tick() override;

		void DoLayout() override;

		void DoPaint() override;

		void EmplaceFrameAttachments() override;

		void EnqueueScopes() override;

		void SetDrawListMask(RHI::DrawListMask& outMask) override;

		void EnqueueDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex) override;

		void SetDrawPackets(RHI::DrawListContext& drawList) override;

		void OnWidgetDestroyed(FWidget* widget) override;

		FPainter* GetPainter() override;

	protected:

		void Init();

		void OnBeforeDestroy() override;

		void OnWindowResized(PlatformWindow* window, u32 newWidth, u32 newHeight) override;
		void OnWindowExposed(PlatformWindow* window) override;

		void UpdateViewConstants();

		PlatformWindow* platformWindow = nullptr;

		RHI::SwapChain* swapChain = nullptr;
		RHI::DrawListTag drawListTag = 0;
		Name attachmentId;

		FusionRenderer* renderer = nullptr;
		FPainter* painter = nullptr;

		FWidget* prevHoveredWidget = nullptr;
		StaticArray<FWidget*, 6> widgetsPressedPerMouseButton{};

		// Previous mouse position in window space
		Vec2 prevMousePos = Vec2();

		FUSION_FRIENDS;
	};

} // namespace CE

#include "FNativeContext.rtti.h"