#include "FusionCore.h"

namespace CE
{

    FStyledWidget::FStyledWidget()
    {

    }

    FStyledWidget::Self& FStyledWidget::UseStyle(FStyle* style)
    {
        if (style && IsOfType(style->GetWidgetClass()))
        {
            m_Style = style;
            m_Style->MakeStyle(*this);
            MarkDirty();
        }
        return *this;
    }

    FStyledWidget::Self& FStyledWidget::UseStyle(const CE::Name& styleKey)
    {
        if (!styleKey.IsValid())
            return *this;

        FFusionContext* context = GetContext();
        if (!context)
            return *this;

        FStyleManager* styleManager = context->GetStyleManager();
        if (!styleManager)
            return *this;

        FStyle* style = styleManager->FindStyle(styleKey);
        return UseStyle(style);
    }

}

