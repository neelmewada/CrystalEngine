#pragma once

namespace CE::Widgets
{

    CLASS()
    class CRYSTALEDITOR_API AssetItemWidget : public CButton
    {
        CE_CLASS(AssetItemWidget, CButton)
    public:

        AssetItemWidget();

        virtual ~AssetItemWidget();

    protected:

		void Construct() override;

        void OnDrawGUI() override;

		void HandleEvent(CEvent* event) override;

	private:

		GUI::GuiStyleState hovered{};

		GUI::GuiStyleState selected{};
    };
    
} // namespace CE

#include "AssetItemWidget.rtti.h"
