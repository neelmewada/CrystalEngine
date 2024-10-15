#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API SceneOutlinerTab : public EditorMinorDockTab
    {
        CE_CLASS(SceneOutlinerTab, EditorMinorDockTab)
    public:

        // - Public API -

    protected:

        SceneOutlinerTab();

        void Construct() override;

        SceneTreeView* treeView = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "SceneOutlinerTab.rtti.h"
