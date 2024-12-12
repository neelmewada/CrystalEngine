#pragma once

namespace CE
{
    class FTreeView;
    class FTreeViewRow;

    CLASS()
    class FUSION_API FTreeViewContainer : public FWidget
    {
        CE_CLASS(FTreeViewContainer, FWidget)
    public:

        // - Public API -

        void SetContextRecursively(FFusionContext* context) override;

        FWidget* HitTest(Vec2 localMousePos) override;

        bool ChildExistsRecursive(FWidget* child) override;

        void ApplyStyleRecursively() override;

        void HandleEvent(FEvent* event) override;

        void CalculateIntrinsicSize() override;

        void PlaceSubWidgets() override;

        bool SupportsMouseEvents() const override { return true; }

        bool SupportsKeyboardEvents() const override { return true; }

        bool CapturesMouseWheel() const override { return true; }

    protected:

        FTreeViewContainer();

        void Construct() override;

        Rect GetVerticalScrollBarRect();

        bool IsVerticalScrollVisible();

    	void OnPaint(FPainter* painter) override;

        void OnPostComputeLayout() override;

        void OnModelUpdate();

        Self& TreeView(FTreeView* treeView)
        {
            this->treeView = treeView;
            return *this;
        }

        typedef StableDynamicArray<FTreeViewRow*, 64, false> TreeViewRowList;

        FTreeView* treeView = nullptr;
        TreeViewRowList children;
        HashSet<FModelIndex> expandedRows;
        Vec2 modelUpdateComputedSize;
        f32 totalRowHeight = 0;
        bool isScrollHovered = false;

    public: // - Fusion Properties -

        // - ScrollBar -

        Self& NormalizedScrollY(f32 value);
        f32 NormalizedScrollY();

        FUSION_WIDGET;
        friend class FTreeView;
        friend class FTreeViewStyle;
    };
    
}

#include "FTreeViewContainer.rtti.h"
