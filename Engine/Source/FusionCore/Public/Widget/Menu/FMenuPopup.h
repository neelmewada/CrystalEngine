#pragma once

namespace CE
{
    class FMenuItem;

    CLASS()
    class FUSIONCORE_API FMenuPopup : public FPopup
    {
        CE_CLASS(FMenuPopup, FPopup)
    public:

        FMenuPopup();

        u32 GetMenuItemCount() const { return menuItems.GetSize(); }

        FMenuItem* GetMenuItem(u32 index) const { return menuItems[index]; }

    protected:

        void Construct() override final;

        FIELD()
        Array<FMenuItem*> menuItems;

        FVerticalStack* container = nullptr;

    public: // - Fusion Properties -

        FUSION_PROPERTY_WRAPPER(Gap, container);

        template <typename... TArgs> requires TMatchAllBaseClass<FWidget, TArgs...>::Value and (sizeof...(TArgs) > 0)
        Self& Content(const TArgs&... widgets)
        {
            using TupleType = std::tuple<const TArgs&...>;
            TupleType args = { widgets... };

            constexpr_for<0, sizeof...(TArgs), 1>([&](auto i)
                {
                    using ArgTypeBase = std::tuple_element_t<i(), TupleType>;
                    using ArgType = std::remove_cvref_t<ArgTypeBase>;

					if constexpr (TIsBaseClassOf<FMenuItem, ArgType>::Value)
					{
                        const_cast<ArgType*>(&std::get<i()>(args))->menuOwner = this;
                        menuItems.Add(const_cast<ArgType*>(&std::get<i()>(args)));
					}
                    if constexpr (TIsBaseClassOf<FWidget, ArgType>::Value)
                    {
                        container->AddChild(const_cast<ArgType*>(&std::get<i()>(args)));
                    }
                });

            return *this;
        }

        FUSION_WIDGET;
    };
    
}

#include "FMenuPopup.rtti.h"
