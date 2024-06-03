#include "FusionCore.h"

namespace CE
{

	FFusionContext::FFusionContext()
	{
		layoutManager = CreateDefaultSubobject<FLayoutManager>("LayoutManager");
		layoutManager->context = this;
	}

	FFusionContext::~FFusionContext()
	{

	}

	bool FFusionContext::IsNativeContext() const
	{
		return IsOfType<FNativeContext>();
	}

	void FFusionContext::Tick()
	{

	}

} // namespace CE
