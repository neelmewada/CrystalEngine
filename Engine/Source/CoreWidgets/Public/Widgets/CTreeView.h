#pragma once

namespace CE::Widgets
{
	CLASS()
	class COREWIDGETS_API CTreeItemView : public CWidget
	{
		CE_CLASS(CTreeItemView, CWidget)
	public:

		CTreeItemView();

		virtual ~CTreeItemView();

		bool IsContainer() override { return true; }

		bool RequiresLayoutCalculation() override { return true; }

	protected:

		virtual void OnDrawGUI() override;

	};

    CLASS()
    class COREWIDGETS_API CTreeView : public CWidget
    {
        CE_CLASS(CTreeView, CWidget)
    public:

        CTreeView();
        virtual ~CTreeView();

		bool IsContainer() override { return true; }

		inline bool AlwaysShowVerticalScroll() const { return alwaysShowVerticalScroll; }
		inline void SetAlwaysShowVerticalScroll(bool set) { alwaysShowVerticalScroll = set; }

    protected:

        virtual void OnDrawGUI() override;


		FIELD()
		CDataModel* model = nullptr;

		FIELD()
		b8 alwaysShowVerticalScroll = false;
    };
    
} // namespace CE

#include "CTreeView.rtti.h"
