#include "FusionCoreTest.h"

namespace LayoutTests
{

    void TerminalWidget::Construct()
    {
	    
    }

    struct FBuilder
    {
	    
    };

    template<typename T>
    struct TIsValidForEachLamda
    {
        using Traits = FunctionTraits<T>;
        using ReturnType = typename Traits::ReturnType;
        using Arg0 = typename Traits::template Arg<0>::Type;

        static_assert(Traits::NumArgs == 1);

        static constexpr bool Value = (Traits::NumArgs == 1)
    		and TIsBaseClassOf<FWidget, std::remove_cvref_t<ReturnType>>::Value
    		and std::is_reference_v<ReturnType>
    		and TIsSameType<Arg0, int>::Value;
    };

    void LayoutTestWidget::Construct()
    {

        Child(
            FAssignNew(FVerticalStack, rootBox)
            .ContentHAlign(HAlign::Fill)
            .Padding(5)
            .Name("RootBox")
            (
                FAssignNew(FHorizontalStack, hStack1)
                .ContentVAlign(VAlign::Center)
                .Padding(5, 2.5f)
                .Name("HStack1")
                (
                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(10),

                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(30),

                    FNew(TerminalWidget)
                    .VAlign(VAlign::Fill)
                    .MinHeight(15)
                    .FillWidth(1.0),

                    FNew(TerminalWidget)
                    .VAlign(VAlign::Top)
                    .MinHeight(15)
                    .FillWidth(2.0),

                    FNew(TerminalWidget)
                    .VAlign(VAlign::Center)
                    .MinHeight(15)
                    .FillWidth(1.0),

                    FNew(TerminalWidget)
                    .VAlign(VAlign::Bottom)
                    .MinHeight(15)
                    .FillWidth(1.0)
                ),

                FAssignNew(FHorizontalStack, hStack2)
                .ContentVAlign(VAlign::Center)
                .Padding(0)
                .Name("HStack2")
                (
                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(10)
                    .Margin(Vec4(0, 0, 5, 0)),

                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(30)
                    .FillWidth(1.0)
                    .Margin(Vec4(0, 0, 5, 0)),

                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(10)
                    .FillWidth(3.0)
                ),

                FAssignNew(FHorizontalStack, hStack3)
                .FillHeight(1.0f)
                .HAlign(HAlign::Center)
                .Name("HStack3")
                (
                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(10),

                    FNew(TerminalWidget)
                    .MinWidth(10)
                    .MinHeight(30)
                )
            )
        );
    }

}
