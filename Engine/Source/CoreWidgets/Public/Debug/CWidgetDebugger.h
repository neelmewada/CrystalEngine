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

		void DrawLayoutRectAtCenter(const String& title, bool isEnabled, const Color& color, const Color& clearColor, const Vec4& values, const Rect& screenRect);

		void DrawComputedFieldsTable();
		void DrawPropertiesTable();

		bool isShown = false;
		bool enableLayoutDebugMode = false;
		CDebugBackgroundFilter hoveredLayoutItem = CDebugBackgroundFilter::None;

		CWidget* debugWidget = nullptr;
		bool foundWidgetInHierarchy = true;

		Array<CWindow*> registeredWindows{};

		friend class CWidget;
		friend class CWindow;
	};

} // namespace CE::Widgets

#include "CWidgetDebugger.rtti.h"
