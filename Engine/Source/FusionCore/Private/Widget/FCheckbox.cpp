#include "FusionCore.h"

namespace CE
{

    FCheckbox::FCheckbox()
    {

    }

    void FCheckbox::Construct()
    {
        Super::Construct();
        
        FBrush icon = FBrush("/Engine/Resources/Icons/CheckMark");

        Child(
            FAssignNew(FImage, checkmark)
            .Background(icon)
            .Width(12)
            .Height(12)
            .Visible(m_Checked)
        );

        Padding(Vec4(1, 1, 1, 1) * 4);
    }

    void FCheckbox::OnFusionPropertyModified(const CE::Name& propertyName)
    {
        Super::OnFusionPropertyModified(propertyName);

        thread_local CE::Name checked = "Checked";

        if (checked == propertyName)
        {
            checkmark->Visible(m_Checked);
        }
    }

    void FCheckbox::OnClick()
    {
        Checked(!Checked());

        m_OnCheckChanged(this);
    }
    
}

