#pragma once

namespace CE::Widgets
{
	class CToolBar;

	CLASS()
	class COREWIDGETS_API CToolBarItem : public CWidget
	{
		CE_CLASS(CToolBarItem, CWidget)
	public:

		CToolBarItem();
		virtual ~CToolBarItem();

		bool IsContainer() override { return true; }

		inline const String& GetTitle() const { return title; }
		inline void SetTitle(const String& title) { this->title = title; }

	protected:

		void OnDrawGUI() override;


		CTextureID icon = nullptr;

		FIELD()
		String title = "";
	};
    
} // namespace CE::Widgets

#include "CToolBarItem.rtti.h"
