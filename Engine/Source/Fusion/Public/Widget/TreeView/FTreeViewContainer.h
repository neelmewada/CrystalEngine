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

        u32 GetChildCount() const { return children.GetSize(); }

        FTreeViewRow* GetChild(u32 index) const { return children[index]; }

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

        FTreeView* treeView = nullptr;

        Self& TreeView(FTreeView* treeView)
        {
            this->treeView = treeView;
            return *this;
        }

        Array<FTreeViewRow*> children;

    public: // - Fusion Properties - 

        FUSION_WIDGET;
        friend class FTreeView;
    };
    
}

#include "FTreeViewContainer.rtti.h"
