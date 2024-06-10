#include "FusionCore.h"

namespace CE
{

	FNativeContext::FNativeContext()
	{
		isIsolatedContext = true;
	}

	FNativeContext::~FNativeContext()
	{
		if (platformWindow)
		{
			
		}
	}

	FNativeContext* FNativeContext::Create(PlatformWindow* platformWindow, const String& name, FFusionContext* parentContext)
	{
		Object* outer = parentContext;
		if (outer == nullptr)
		{
			outer = FusionApplication::TryGet();
		}
		if (outer == nullptr)
		{
			return nullptr;
		}

		FNativeContext* nativeContext = CreateObject<FNativeContext>(outer, name);
		nativeContext->platformWindow = platformWindow;
		if (parentContext)
		{
			parentContext->AddChildContext(nativeContext);
		}
		nativeContext->Init();
		return nativeContext;
	}

	void FNativeContext::Init()
	{
		renderer = CreateObject<FusionRenderer>(this, "FusionRenderer");
	}

	void FNativeContext::Tick()
	{
		ZoneScoped;

		Super::Tick();

		// TODO: Rendering
	}

} // namespace CE
