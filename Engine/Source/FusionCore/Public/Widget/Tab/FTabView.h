#pragma once

namespace CE
{

    CLASS()
    class FUSIONCORE_API FTabView : public FStyledWidget
    {
        CE_CLASS(FTabView, FStyledWidget)
    public:

        FTabView();

    protected:

        void Construct() override final;

        template<typename... TArgs>
        struct TValidate_TabItems : TFalseType
        {};

        template<typename T>
        struct TValidate_TabItems<T> : TBoolConst<TIsBaseClassOf<FTabItem, T>::Value>
        {};

        template<typename TFirst, typename... TRest>
        struct TValidate_TabItems<TFirst, TRest...> : TBoolConst<TValidate_TabItems<TFirst>::Value and TValidate_TabItems<TRest...>::Value>
        {};

    public:

        bool TryAddChild(FWidget* child) override;

        void AddTabItem(FTabItem* item);

        u32 GetTabItemCount() const { return tabItems.GetSize(); }

        FTabItem* GetTabItem(u32 index) { return tabItems[index]; }

        void SelectTab(int tabIndex);
        void SelectTab(FTabItem* tabItem);

        FStyledWidget* GetContainer() const { return container; }

        FStackBox* GetTabWell() const { return tabWell; }

        bool HasContainer() const { return container != nullptr; }

    public: // - Fusion Properties - 

        template <typename... TArgs> requires TValidate_Children<TArgs...>::Value
        FTabView& TabItems(const TArgs&... items)
        {
            using TupleType = std::tuple<const TArgs&...>;
            TupleType args = { items... };

            constexpr_for<0, sizeof...(TArgs), 1>([&](auto i)
                {
                    using ArgTypeBase = std::tuple_element_t<i(), TupleType>;
                    using ArgType = std::remove_cvref_t<ArgTypeBase>;
                    
                    this->AddTabItem(Object::CastTo<FTabItem>(const_cast<ArgType*>(&std::get<i()>(args))));
                });
            return *this;
        }

    private:

        bool isConstructed = false;

        FHorizontalStack* tabWell = nullptr;
        FStyledWidget* container = nullptr;

        FIELD()
        Array<FTabItem*> tabItems;

        int activeTabIndex = 0;

    public:

        FUSION_PROPERTY_WRAPPER2(Background, container, ContainerBackground);
        FUSION_PROPERTY_WRAPPER2(BorderColor, container, ContainerBorderColor);
        FUSION_PROPERTY_WRAPPER2(BorderWidth, container, ContainerBorderWidth);

        FUSION_PROPERTY_WRAPPER2(FillRatio, container, ContainerFillRatio);

        Self& ContainerBorder(const Color& borderColor, f32 borderWidth = 1.0f)
        {
            return (*this)
        		.ContainerBorderColor(borderColor)
        		.ContainerBorderWidth(borderWidth);
        }

        FUSION_WIDGET;
    };
    
}

#include "FTabView.rtti.h"
