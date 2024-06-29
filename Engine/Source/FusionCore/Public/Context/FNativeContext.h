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

		bool IsFocused() const override;

		void TickInput() override;

		void DoLayout() override;

		void DoPaint() override;

		void PushNativePopup(FPopup* popup, Vec2 globalPosition, Vec2 size = Vec2()) override;

		void EmplaceFrameAttachments() override;

		void EnqueueScopes() override;

		void SetDrawListMask(RHI::DrawListMask& outMask) override;

		void EnqueueDrawPackets(RHI::DrawListContext& drawList, u32 imageIndex) override;

		void SetDrawPackets(RHI::DrawListContext& drawList) override;

		void OnWidgetDestroyed(FWidget* widget) override;

		FPainter* GetPainter() override;

		Vec2 GlobalToScreenSpacePosition(Vec2 pos) override;
		Vec2 ScreenToGlobalSpacePosition(Vec2 pos) override;

	protected:

		bool WindowDragHitTest(PlatformWindow* window, Vec2 position);

		void Init();

		void OnQueuedDestroy() override;

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

		KeyModifier keyModifierStates{};
		BitSet<128> keyPressStates{};

		FWidget* draggedWidget = nullptr;
		FWidget* prevHoveredWidget = nullptr;
		StaticArray<FWidget*, 6> widgetsPressedPerMouseButton{};

		// Previous mouse position in window space
		Vec2 prevMousePos = Vec2();

		FUSION_FRIENDS;
	};

} // namespace CE

#include "FNativeContext.rtti.h"