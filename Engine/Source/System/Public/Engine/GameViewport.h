#pragma once

namespace CE
{
	CLASS()
	class SYSTEM_API GameViewport : public Object
	{
		CE_CLASS(GameViewport, Object)
	public:
		GameViewport();

		void Initialize(RPI::RenderViewport* rpiViewport);

		void Shutdown();

		inline u32 GetWidth() const { return rpiViewport->GetRenderTarget()->GetWidth(); }

		inline u32 GetHeight() const { return rpiViewport->GetRenderTarget()->GetHeight(); }

		inline bool IsNativeSurface() const { return rpiViewport->IsNativeViewport(); }

		inline RHI::RenderTarget* GetRenderTarget() const { return rpiViewport->GetRenderTarget(); }

	protected:

		RPI::RenderViewport* rpiViewport = nullptr;
	};
    
} // namespace CE

#include "GameViewport.rtti.h"