#pragma once

namespace CE::Widgets
{
	class CWidget;

	CLASS(Abstract)
	class CRYSTALWIDGETS_API CBehavior : public Object
	{
		CE_CLASS(CBehavior, Object)
	public:

		CBehavior();
		virtual ~CBehavior();

		virtual void HandleEvent(CEvent* event) {}

		virtual void OnPaint(CPainter* painter) {}

		virtual void OnPaintOverlay(CPainter* painter) {}

		virtual Vec4 GetExtraRootPadding() { return Vec4(); }

	protected:

		FIELD()
		CWidget* self = nullptr;

		friend class CWidget;
	};

} // namespace CE::Widgets

#include "CBehavior.rtti.h"