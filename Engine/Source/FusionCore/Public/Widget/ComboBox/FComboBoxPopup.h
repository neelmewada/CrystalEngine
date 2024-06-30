#pragma once


namespace CE
{
    class FComboBox;
    class FVerticalStack;

    CLASS()
    class FUSIONCORE_API FComboBoxPopup : public FPopup
    {
        CE_CLASS(FComboBoxPopup, FPopup)
    public:

        FComboBoxPopup();

        // - Public API -

        FVerticalStack* GetContentStack() const { return content; }

    protected:

        void Construct() override;

        void OnAttachedToParent(FWidget* parent) override;

        void AddContentChild(FWidget* child);

        FIELD()
        FComboBox* comboBox = nullptr;

        FIELD()
        FVerticalStack* content = nullptr;

    protected: // - Fusion Fields -

    public: // - Fusion Properties - 

        template<typename... TWidget> requires TMatchAllBaseClass<FWidget, TWidget...>::Value
        Self& Content(TWidget&... widgets)
        {
            std::initializer_list<FWidget*> list = { &widgets... };

            for (FWidget* widget : list)
            {
                AddContentChild(widget);
            }

            return *this;
        }

        FUSION_WIDGET;
        friend class FComboBox;
        friend class FComboBoxItem;
    };
    
}

#include "FComboBoxPopup.rtti.h"
