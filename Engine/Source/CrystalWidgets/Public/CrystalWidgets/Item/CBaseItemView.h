#pragma once

namespace CE::Widgets
{
    class CItemSelectionModel;

    CLASS(Abstract)
    class CRYSTALWIDGETS_API CBaseItemView : public CWidget
    {
        CE_CLASS(CBaseItemView, CWidget)
    public:

        CBaseItemView();
        virtual ~CBaseItemView();

        CItemSelectionModel* GetSelectionModel() const { return selectionModel; }

        void SetSelectionModel(CItemSelectionModel* selectionModel);

        CBaseItemModel* GetModel() const { return model; }

        void SetModel(CBaseItemModel* model);

    protected:

    private:

        FIELD()
        CItemSelectionModel* selectionModel = nullptr;

        FIELD()
        CItemSelectionModel* defaultSelectionModel = nullptr;

        FIELD()
        CBaseItemModel* model = nullptr;

    };
    
} // namespace CE::Widgets

#include "CBaseItemView.rtti.h"