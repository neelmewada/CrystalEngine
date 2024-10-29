#include "Fusion.h"

namespace CE
{

    FExpandableSectionStyle::FExpandableSectionStyle()
    {

    }

    FExpandableSectionStyle::~FExpandableSectionStyle()
    {
        
    }

    SubClass<FWidget> FExpandableSectionStyle::GetWidgetClass() const
    {
        return FExpandableSection::StaticType();
    }

    void FExpandableSectionStyle::MakeStyle(FWidget& widget)
    {
        FExpandableSection& section = widget.As<FExpandableSection>();

        FButton& header = *section.header;

        header
    		.Background(headerBackground)
			.Border(headerBorderColor, headerBorder)
			.CornerRadius(headerCornerRadius)
    	;

        FStyledWidget& content = *section.content;

        content
			.Background(contentBackground)
			.Border(contentBorderColor, contentBorder)
			.CornerRadius(contentCornerRadius)
        ;
    }

} // namespace CE

