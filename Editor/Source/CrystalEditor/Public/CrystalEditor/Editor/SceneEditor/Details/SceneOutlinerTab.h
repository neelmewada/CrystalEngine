#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API SceneOutlinerTab : public EditorMinorDockTab, ISceneCallbacks
    {
        CE_CLASS(SceneOutlinerTab, EditorMinorDockTab)
    public:

        // - Public API -

        void SetScene(CE::Scene* scene);

    protected:

        SceneOutlinerTab();

        void Construct() override;

        void OnBeforeDestroy() override;

        void OnSceneHierarchyUpdated(CE::Scene* scene) override;

        SceneTreeView* treeView = nullptr;
        SceneTreeViewModel* treeViewModel = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
        friend class SceneEditor;
    };
    
}

#include "SceneOutlinerTab.rtti.h"
