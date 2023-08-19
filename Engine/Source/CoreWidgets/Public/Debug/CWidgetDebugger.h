#pragma once

namespace CE::Widgets
{
    CLASS()
	class COREWIDGETS_API CWidgetDebugger : public Object
	{
		CE_CLASS(CWidgetDebugger, Object)
	public:

		void RenderGUI();

		inline void Show() { isShown = true; }
		inline void Hide() { isShown = false; }

		inline bool IsShown() { return isShown; }
		inline bool IsHidden() { return !isShown; }

		void SetDebugWidget(CWidget* target);

	private:

		void DrawWidgetTreeEntry(CWidget* widget);

		void DrawLayoutRectAtCenter(const String& title, const Color& color, const Vec4& values, const Vec2& rectSize);

		bool isShown = false;
		bool enableLayoutDebugMode = false;

		CWidget* debugWidget = nullptr;
		bool foundWidgetInHierarchy = true;

		Array<CWindow*> registeredWindows{};

		friend class CWidget;
		friend class CWindow;
	};

} // namespace CE::Widgets

#include "CWidgetDebugger.rtti.h"
