#include "FusionCore.h"

namespace CE
{

	FFusionContext::FFusionContext()
	{

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

		for (FFusionContext* childContext : childContexts)
		{
			childContext->Tick();
		}
	}

	void FFusionContext::DoLayout()
	{
		ZoneScoped;

		if (owningWidget && layoutDirty)
		{
			owningWidget->PrecomputeIntrinsicSize();

			owningWidget->computedPosition = Vec2();
			owningWidget->computedSize = availableSize;
			
			owningWidget->PlaceSubWidgets();

			layoutDirty = false;
		}
	}

	void FFusionContext::SetOwningWidget(FWidget* widget)
	{
		this->owningWidget = widget;
		this->owningWidget->context = this;
	}

	void FFusionContext::MarkLayoutDirty()
	{
		layoutDirty = true;
		dirty = true;
	}

	void FFusionContext::MarkDirty()
	{
		dirty = true;
	}

	void FFusionContext::AddChildContext(FFusionContext* context)
	{
		if (childContexts.Exists(context))
			return;

		context->parentContext = this;
		childContexts.Add(context);
	}

	void FFusionContext::RemoveChildContext(FFusionContext* context)
	{
		if (!childContexts.Exists(context))
			return;

		context->parentContext = nullptr;
		childContexts.Remove(context);
	}

	void FFusionContext::SetStyleManager(FStyleManager* styleManager)
	{
		this->styleManager = styleManager;
		MarkDirty();
	}

	FStyleManager* FFusionContext::GetStyleManager()
	{
		if (styleManager == nullptr && parentContext)
		{
			return parentContext->GetStyleManager();
		}
		return styleManager;
	}

} // namespace CE
