#pragma once

namespace CE::Widgets
{
	class CWidget;

	CLASS()
	class CRYSTALWIDGETS_API CBehavior : public Object
	{
		CE_CLASS(CBehavior, Object)
	public:

		CBehavior();
		virtual ~CBehavior();

	protected:

	};

} // namespace CE::Widgets

#include "CBehavior.rtti.h"