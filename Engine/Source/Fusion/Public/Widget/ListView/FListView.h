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

        typedef ScriptDelegate<FListItemWidget&(FListItem*, FListView*)> GenerateRowCallback;

        FListView();

        bool HasScrollBox() const { return scrollBox != nullptr; }

        FScrollBox& GetScrollBox() const { return *scrollBox; }

    protected:

        void Construct() override final;

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        void RegenerateRows();

        FIELD()
        GenerateRowCallback m_OnGenerateRow;

        FVerticalStack* content = nullptr;
        FScrollBox* scrollBox = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(FSelectionMode, SelectionMode);

        FUSION_DATA_PROPERTY(Array<FListItem*>, ItemList);

        Self& OnGenerateRow(const GenerateRowCallback& callback);

        FUSION_WIDGET;
    };
    
}

#include "FListView.rtti.h"
