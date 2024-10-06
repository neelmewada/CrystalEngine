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

        void Construct() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "SceneEditor.rtti.h"
