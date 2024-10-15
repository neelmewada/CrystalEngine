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

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "SceneTreeView.rtti.h"
