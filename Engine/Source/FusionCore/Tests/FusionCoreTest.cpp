#include "FusionCoreTest.h"

namespace ConstructTests
{

    ComplexWidget::ComplexWidget()
    {

    }

    void ComplexWidget::Construct()
    {
        Child(
            FAssignNew(FStackBox, rootBox)
            .Direction(FStackBoxDirection::Vertical)    // ROOT (Vertical Group)
			.ContentHAlign(HAlign::Center)
            .Padding(10, 10)
			.Name("RootStackBox")
            (
                FNew(FStackBox)
				.Direction(FStackBoxDirection::Horizontal)    // Root / Horizontal
				.Padding(0, 0)
				.Name("Stack0")
				(
                    FNew(FStackBox)
                    .Direction(FStackBoxDirection::Vertical)     // Root / Horizontal / Vertical
                    .Padding(5, 2)
                    .Name("Stack0_0")
                    (
                        FNew(FNullWidget)
						.Padding(1),

                        FNew(FNullWidget)
						.Padding(2)
                    )
                )
            )
        );
    }

} // namespace ConstructTests

