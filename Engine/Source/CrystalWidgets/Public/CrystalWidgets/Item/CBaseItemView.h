#pragma once

namespace CE::Widgets
{
    class CItemSelectionModel;
    class CBaseItemDelegate;

    ENUM()
    enum class CItemSelectionType
    {
	    SelectItem,
        SelectRow
    };
    ENUM_CLASS(CItemSelectionType);

    ENUM()
    enum class CItemSelectionMode
    {
	    SingleSelection,
        MultiSelection,
        NoSelection,
        ExtendedSelection
    };
    ENUM_CLASS(CItemSelectionMode);

    ENUM(Flags)
    enum class CItemViewHeaderFlags
    {
	    None = 0,
        Resizable = BIT(0),

    };
    ENUM_CLASS_FLAGS(CItemViewHeaderFlags);

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

        void SetColumFixedWidth(u32 column, f32 width);
        void SetColumnResizable(u32 column, f32 widthRatio);


    protected:

        FUNCTION()
        void OnSelectionModelUpdated();

        Rect GetVerticalScrollBarRect();

        void CalculateRowHeights(Array<f32>& outHeights, const CModelIndex& parent = {});

        void PaintRows(CPainter* painter, const Rect& regionRect, int indentLevel, const CModelIndex& parent = {});

        void HandleEvent(CEvent* event) override;

        void Construct() override;

        void OnPaint(CPaintEvent* paintEvent) override;

        void OnPaintOverlay(CPaintEvent* paintEvent) override;


        FIELD()
        CItemSelectionType selectionType = CItemSelectionType::SelectItem;

        FIELD()
        CItemSelectionMode selectionMode = CItemSelectionMode::MultiSelection;

        FIELD()
        Array<CItemViewHeaderFlags> columnHeaderFlags{};

        FIELD()
        b8 canDrawColumnHeader = true;

        FIELD()
        b8 canDrawRowHeader = true;

        FIELD()
        b8 canExpandRows = true;

    private:

        Vec2 scrollOffset = Vec2(0, 0);

        FIELD()
        CItemSelectionModel* selectionModel = nullptr;

        FIELD()
        CItemSelectionModel* defaultSelectionModel = nullptr;

        FIELD()
        CBaseItemModel* model = nullptr;

        FIELD()
        CBaseItemDelegate* delegate = nullptr;

        int hoveredColumnHeader = -1;
        int hoveredRowHeader = -1;
        int expandableColumn = -1;

        b8 modelUpdated = true;
        b8 recalculateRows = true;

        b8 isVerticalScrollHovered = false;
        b8 isVerticalScrollPressed = false;

        Color alternateBgColor{};

        HashSet<CModelIndex> expandedRows{};
        CStyle cellStyle{};
        CStyle cellHoveredStyle{};
        CStyle cellSelectedStyle{};

        CModelIndex hoveredCell{};
        KeyModifier keyModifier{};

        Array<f32> columnWidths{};
        Array<f32> columnWidthRatios{};

        f32 columnHeaderHeight = 0.0f;
        HashMap<CModelIndex, Array<f32>> rowHeightsByParent{};
        f32 totalContentHeight = 0;

        Vec2 localMousePos = Vec2(-1, -1);
        b8 isMouseHovering = false;
        b8 isMouseLeftClick = false;
    };
    
} // namespace CE::Widgets

#include "CBaseItemView.rtti.h"