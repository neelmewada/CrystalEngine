#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API SceneEditor : public EditorDockTab
    {
        CE_CLASS(SceneEditor, EditorDockTab)
    public:

        // - Public API -

        bool CanBeClosed() const override { return false; }

    protected:

        SceneEditor();

        void LoadSandboxScene();

        void Construct() override;

        FSplitBox* rootSplitBox = nullptr;

    private:

        void ConstructMenuBar();
        void ConstructDockspaces();

        EditorMinorDockspace* right = nullptr;
        EditorMinorDockspace* center = nullptr;
        EditorMinorDockspace* bottom = nullptr;

        EditorViewportTab* viewportTab = nullptr;

        SceneOutlinerTab* sceneOutlinerTab = nullptr;

        // Sandbox
        CE::Scene* sandboxScene = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "SceneEditor.rtti.h"
