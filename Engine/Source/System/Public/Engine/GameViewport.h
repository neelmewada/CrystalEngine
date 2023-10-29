#pragma once

namespace CE
{
	CLASS()
	class SYSTEM_API GameViewport : public Object
	{
		CE_CLASS(GameViewport, Object)
	public:
		GameViewport();

		void Initialize(RHI::RenderTarget* renderTarget);

		void Shutdown();

		inline u32 GetWidth() const { return renderTarget->GetWidth(); }

		inline u32 GetHeight() const { return renderTarget->GetHeight(); }

		inline bool IsNativeSurface() const { return renderTarget->IsViewportRenderTarget(); }

		inline RHI::RenderTarget* GetRenderTarget() const { return renderTarget; }

	protected:

		RHI::RenderTarget* renderTarget = nullptr;
	};
    
} // namespace CE

#include "GameViewport.rtti.h"