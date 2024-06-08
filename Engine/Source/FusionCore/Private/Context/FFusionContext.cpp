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
		ZoneScoped;

		DoLayout();
	}

	void FFusionContext::DoLayout()
	{
		if (owningWidget)
		{
			
		}
	}

	void FFusionContext::SetOwningWidget(FWidget* widget)
	{
		this->owningWidget = widget;
		this->owningWidget->context = this;
	}

} // namespace CE
