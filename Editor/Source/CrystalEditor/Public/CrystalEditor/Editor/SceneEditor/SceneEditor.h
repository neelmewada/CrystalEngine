#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API SceneEditor : public EditorDockTab, public ISceneSubsystemCallbacks
    {
        CE_CLASS(SceneEditor, EditorDockTab)
    public:

        // - Public API -

        bool CanBeClosed() const override { return false; }

    protected:

        SceneEditor();

        void LoadSandboxScene();

        void Construct() override;

        void OnBeforeDestroy() override;

        void OnSceneLoaded(CE::Scene* scene) override;

        FUNCTION()
        void OnSelectionChanged(FItemSelectionModel* selectionModel);

        FSplitBox* rootSplitBox = nullptr;

    private:

        void ConstructMenuBar();
        void ConstructDockspaces();

        EditorMinorDockspace* rightTop = nullptr;
        EditorMinorDockspace* rightBottom = nullptr;
        EditorMinorDockspace* center = nullptr;
        EditorMinorDockspace* bottom = nullptr;

        EditorViewportTab* viewportTab = nullptr;

        SceneOutlinerTab* sceneOutlinerTab = nullptr;

        DetailsTab* detailsTab = nullptr;

        // Sandbox
        CE::Scene* sandboxScene = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "SceneEditor.rtti.h"
