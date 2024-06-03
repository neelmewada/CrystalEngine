#pragma once

namespace ConstructTests
{

    inline ComplexWidget::ComplexWidget()
    {

    }

    inline void ComplexWidget::Construct()
    {
	    

        ChildSlot(
            FAssignNew(rootBox, FStackBox)
            .Direction(FStackBoxDirection::Vertical) // ROOT (Vertical Group)

            + FStackBox::Slot()
            .Padding(10, 10)        
            .VAlign(VAlign::Center)
            (
                FNew(FStackBox)
                .Direction(FStackBoxDirection::Horizontal) // Root / Horizontal

                + FStackBox::Slot()
                .Padding(0, 0)
                .HAlign(HAlign::Center)
                (
                    FNew(FStackBox)
                    .Direction(FStackBoxDirection::Vertical)  // Root / Horizontal / Vertical

                    + FStackBox::Slot()     
                    .Padding(1)
                    (
                        FNew(FNullWidget)      // Root / Horizontal / Vertical / Null 0
                    )

                    + FStackBox::Slot()     
                    .Padding(2)
                    (
                        FNew(FNullWidget)       // Root / Horizontal / Vertical / Null 1
                    )
                )
            )
        );
    }

} // namespace ConstructTests


namespace LayoutTests
{

    inline LayoutTestWidget::LayoutTestWidget()
    {
    }

    inline void LayoutTestWidget::Construct()
    {
        
    }

}
