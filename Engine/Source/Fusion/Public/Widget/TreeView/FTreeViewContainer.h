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

    protected:

        FTreeViewContainer();

        void Construct() override;

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

    public: // - Fusion Properties - 

        FUSION_WIDGET;
        friend class FTreeView;
    };
    
}

#include "FTreeViewContainer.rtti.h"