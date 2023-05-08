
#include "CoreApplication.h"

namespace CE
{
	PlatformApplication* PlatformApplication::Create()
	{
		return PlatformApplicationImpl::Create();
	}

	void PlatformApplication::Initialize()
	{

	}

	void PlatformApplication::PreShutdown()
	{

	}

	void PlatformApplication::Shutdown()
	{

	}

	void PlatformApplication::Tick()
	{

	}

}

