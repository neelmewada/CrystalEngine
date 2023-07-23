#pragma once

namespace CE::Widgets
{
	ENUM()
	enum class CMenuPosition
	{
		Bottom,
		Top
	};
	ENUM_CLASS_FLAGS(CMenuPosition);

    CLASS()
    class COREWIDGETS_API CMenu : public CWidget
    {
        CE_CLASS(CMenu, CWidget)
    public:

        CMenu();
        virtual ~CMenu();

		void Construct() override;

		bool IsContainer() override { return true; }
		bool RequiresIndependentLayoutCalculation() override;

		void OnAfterComputeStyle() override;

		inline b8 IsShown() const { return isShown; }
		
		void ShowContextMenu();

		void Hide();

		/// Passing `pos` as 0 will automatically pick a position
		void Show(Vec2 pos = Vec2());

    protected:

        virtual void OnDrawGUI() override;

		FIELD()
		b8 isShown = false;

		FIELD()
		CMenuPosition menuPosition = CMenuPosition::Bottom;

		b8 pushShow = false;
		Vec2 showPos = Vec2();
    };
    
} // namespace CE

#include "CMenu.rtti.h"
