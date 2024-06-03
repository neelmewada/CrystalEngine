#include "FusionCore.h"

namespace CE
{

	FNativeContext::FNativeContext()
	{
		isIsolatedContext = true;
	}

	FNativeContext::~FNativeContext()
	{

	}

	FNativeContext* FNativeContext::Create(PlatformWindow* platformWindow, const String& name, FFusionContext* parentContext)
	{
		Object* outer = parentContext;
		if (outer == nullptr)
		{
			outer = FusionApplication::TryGet();
		}

		FNativeContext* nativeContext = CreateObject<FNativeContext>(outer, name);
		nativeContext->platformWindow = platformWindow;
		return nativeContext;
	}

	void FNativeContext::Tick()
	{
		Super::Tick();

	}

} // namespace CE
