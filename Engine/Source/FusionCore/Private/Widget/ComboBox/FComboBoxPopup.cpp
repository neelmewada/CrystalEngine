#include "FusionCore.h"

namespace CE
{

    FComboBoxPopup::FComboBoxPopup()
    {
        m_AutoClose = true;
        m_BlockInteraction = false;
        m_Padding = Vec4(1, 1, 1, 1);
    }

    void FComboBoxPopup::Construct()
    {
	    Super::Construct();

        Child(
            FAssignNew(FVerticalStack, content)
            .ContentHAlign(HAlign::Fill)
            .ContentVAlign(VAlign::Center)
        );
    }

    void FComboBoxPopup::OnAttachedToParent(FWidget* parent)
    {
	    Super::OnAttachedToParent(parent);

        if (parent->IsOfType<FComboBox>())
        {
            comboBox = static_cast<FComboBox*>(parent);
        }
    }

    void FComboBoxPopup::AddContentChild(FWidget* child)
    {
        content->AddChild(child);

        ApplyStyle();
    }

}

