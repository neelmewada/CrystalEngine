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

	protected:

		RHI::RenderTarget* renderTarget = nullptr;
	};
    
} // namespace CE

#include "GameViewport.rtti.h"