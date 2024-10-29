#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API CrystalEditorWindow : public EditorDockspace
    {
        CE_CLASS(CrystalEditorWindow, EditorDockspace)
    public:

        // - Public API -

        static CrystalEditorWindow* Get();

    protected:

        CrystalEditorWindow();

        void Construct() override;

        void OnBeforeDestroy() override;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "CrystalEditorWindow.rtti.h"
