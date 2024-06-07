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
	inline TerminalWidget::TerminalWidget()
	{

	}

	inline void TerminalWidget::Construct()
	{
		
	}

    inline void TerminalWidget::PrecomputeLayoutSize()
    {
        precomputedSize = m_IntrinsicSize;
    }

    inline LayoutTestWidget::LayoutTestWidget()
    {
    }

    inline void LayoutTestWidget::Construct()
    {

        ChildSlot(
            FAssignNew(rootBox, FStackBox)
            .Direction(FStackBoxDirection::Vertical) // RootStack
            .Name("RootStack")

            + FStackBox::Slot()         // RootStack / HorizontalGroup_1
            .AutoSize()
            .Padding(10.f, 5.f)
            (
                FAssignNew(horizontalGroup1, FStackBox)
                .Direction(FStackBoxDirection::Horizontal) 
                .Name("HorizontalGroup_1")

                + FStackBox::Slot()
                .AutoSize()
                .Padding(5.0f)
                (
                    FNew(TerminalWidget)
                    .IntrinsicSize(Vec2(100, 50))
                )

                + FStackBox::Slot()
                .AutoSize()
                .Padding(5.0f)
                (
                    FNew(TerminalWidget)
                    .IntrinsicSize(Vec2(200, 50))
                )
            )

            + FStackBox::Slot()         // RootStack / HorizontalGroup_2
            .AutoSize()
            .Padding(10.f, 5.f)
            (
                FAssignNew(horizontalGroup2, FStackBox)
                .Direction(FStackBoxDirection::Horizontal)
                .Name("HorizontalGroup_2")

                + FStackBox::Slot()
                .AutoSize()
                .Padding(5.0f)
                (
                    FNew(TerminalWidget)
                    .IntrinsicSize(Vec2(150, 75))
                )

                + FStackBox::Slot()
                .AutoSize()
                .Padding(5.0f)
                (
                    FNew(TerminalWidget)
                    .IntrinsicSize(Vec2(250, 75))
                )

                + FStackBox::Slot()
                .AutoSize()
                .Padding(5.0f)
                (
                    FNew(TerminalWidget)
                    .IntrinsicSize(Vec2(50, 75))
                )
            )

            + FStackBox::Slot()         // RootStack / HorizontalGroup_3
            .AutoSize()
            .Padding(10.f, 5.f)
            (
                FAssignNew(horizontalGroup3, FStackBox)
                .Direction(FStackBoxDirection::Horizontal)
                .Name("HorizontalGroup_3")

                + FStackBox::Slot()
                .AutoSize()
                .Padding(5.0f)
                (
                    FNew(TerminalWidget)
                    .IntrinsicSize(Vec2(300, 100))
                )
            )
        );
    }

}
