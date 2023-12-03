#include "System.h"

namespace CE
{
    GameViewport::GameViewport()
    {

    }

	void GameViewport::Initialize(RPI::RenderViewport* rpiViewport)
	{
		this->rpiViewport = rpiViewport;
	}

	void GameViewport::Shutdown()
	{

	}

} // namespace CE
