#pragma once

namespace ConstructTests
{

	inline SimpleWidget::SimpleWidget()
	{
	}

    inline void SimpleWidget::Construct()
    {
        
        ChildSlot(
            FAssignNew(stackBox, FStackBox)
            .Direction(FStackBoxDirection::Vertical)

            + FStackBox::Slot()
            .Padding(5, 2.5f)
            (
                FAssignNew(first, FNullWidget)
            )

            + FStackBox::Slot()
            .Padding(5, 5)
            (
                FNew(FNullWidget)
            )
        );
    }

    inline ComplexWidget::ComplexWidget()
    {

    }

    inline void ComplexWidget::Construct()
    {
	    Super::Construct();

        ChildSlot(
            FAssignNew(rootBox, FStackBox)
            .Direction(FStackBoxDirection::Vertical) // ROOT (Vertical Group)

            + FStackBox::Slot()
            .Padding(10, 10)        // Root / Horizontal
            (
                FNew(FStackBox)
                .Direction(FStackBoxDirection::Horizontal)

                + FStackBox::Slot()
                .Padding(0, 0)      // Root / Horizontal / Vertical
                (
                    FNew(FStackBox)
                    .Direction(FStackBoxDirection::Vertical) 

                    + FStackBox::Slot()     // Root / Horizontal / Vertical / Null 0
                    .Padding(1)
                    (
                        FNew(FNullWidget)
                    )

                    + FStackBox::Slot()     // Root / Horizontal / Vertical / Null 1
                    .Padding(2)
                    (
                        FNew(FNullWidget)
                    )
                )
            )
        );
    }

} // namespace ConstructTests

