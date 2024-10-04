#pragma once

namespace CE
{

    ENUM()
    enum class FSelectionMode
    {
	    None,
        Single
    };
    ENUM_CLASS(FSelectionMode);

    CLASS()
    class FUSION_API FListView : public FStyledWidget
    {
        CE_CLASS(FListView, FStyledWidget)
    public:

        typedef Delegate<FListItemWidget&(FListItem*, FListView*)> GenerateRowCallback;

        bool HasScrollBox() const { return scrollBox != nullptr; }

        FScrollBox& GetScrollBox() const { return *scrollBox; }

        void SelectItem(int index);

    protected:

        FListView();

        void Construct() override final;

        void OnPostComputeLayout() override;

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        void OnItemSelected(FListItemWidget* selectedItem);

        void RegenerateRows();

        FIELD()
        GenerateRowCallback m_GenerateRowDelegate;

        FListViewContainer* content = nullptr;
        FScrollBox* scrollBox = nullptr;

        Array<FListItemWidget*> itemWidgets;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(FSelectionMode, SelectionMode);

        FUSION_PROPERTY_WRAPPER(Gap, content);

        FUSION_DATA_PROPERTY(Array<FListItem*>, ItemList);

        Self& GenerateRowDelegate(const GenerateRowCallback& callback);

        FUSION_WIDGET;
        friend class FListItemWidget;
        friend class FListViewStyle;
    };
    
}

#include "FListView.rtti.h"
