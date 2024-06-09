#pragma once

namespace CE
{
	class FWindow;
	class FLayoutManager;
	class FWidget;
	class FusionRenderer;
	class FFusionContext;

	CLASS()
	class FUSIONCORE_API FNativeContext final : public FFusionContext
	{
		CE_CLASS(FNativeContext, FFusionContext)
	public:

		FNativeContext();

		virtual ~FNativeContext();

		static FNativeContext* Create(PlatformWindow* platformWindow, const String& name, FFusionContext* parentContext = nullptr);

		void Tick() override;

	protected:

		void Init();

		PlatformWindow* platformWindow = nullptr;

		RHI::SwapChain* swapChain = nullptr;
		RHI::DrawListTag drawListTag = 0;

		FusionRenderer* renderer = nullptr;

		FUSION_FRIENDS;
	};

} // namespace CE

#include "FNativeContext.rtti.h"