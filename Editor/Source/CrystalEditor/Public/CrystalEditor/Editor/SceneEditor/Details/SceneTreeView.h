#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API SceneTreeView : public FTreeView
    {
        CE_CLASS(SceneTreeView, FTreeView)
    public:

        // - Public API -

    protected:

        SceneTreeView();

        void Construct() override;

        FTreeViewRow& GenerateRow(const FModelIndex& modelIndex);

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "SceneTreeView.rtti.h"
