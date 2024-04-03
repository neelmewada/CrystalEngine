#pragma once

namespace CE::Widgets
{
    class CItemSelectionModel;
    class CBaseItemDelegate;

    CLASS(Abstract)
    class CRYSTALWIDGETS_API CBaseItemView : public CWidget
    {
        CE_CLASS(CBaseItemView, CWidget)
    public:

        CBaseItemView();
        virtual ~CBaseItemView();

        CItemSelectionModel* GetSelectionModel() const { return selectionModel; }

        Vec2 CalculateIntrinsicSize(f32 width, f32 height) override;

        void SetSelectionModel(CItemSelectionModel* selectionModel);

        CBaseItemModel* GetModel() const { return model; }

        void SetModel(CBaseItemModel* model);

        CBaseItemDelegate* GetDelegate() const { return delegate; }

        void SetDelegate(CBaseItemDelegate* delegate);

        bool IsContainer() const final { return false; }

    protected:

        void HandleEvent(CEvent* event) override;

        void Construct() override;

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnPaintOverlay(CPaintEvent* paintEvent) override;

    private:

        FIELD()
        CItemSelectionModel* selectionModel = nullptr;

        FIELD()
        CItemSelectionModel* defaultSelectionModel = nullptr;

        FIELD()
        CBaseItemModel* model = nullptr;

        FIELD()
        CBaseItemDelegate* delegate = nullptr;

    };
    
} // namespace CE::Widgets

#include "CBaseItemView.rtti.h"