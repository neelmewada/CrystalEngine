#include "System.h"

namespace CE
{
    GameViewport::GameViewport()
    {

    }

	void GameViewport::Initialize(RHI::RenderTarget* renderTarget)
	{
		this->renderTarget = renderTarget;
	}

} // namespace CE
