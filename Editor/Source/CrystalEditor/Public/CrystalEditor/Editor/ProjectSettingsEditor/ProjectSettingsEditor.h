#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API ProjectSettingsEditor : public EditorDockTab
    {
        CE_CLASS(ProjectSettingsEditor, EditorDockTab)
    protected:

        ProjectSettingsEditor();

        void Construct() override;

    public: // - Public API -

    protected: // - Internal -

        void OnSettingsItemClicked(int index);

        FVerticalStack* left = nullptr;
        FVerticalStack* right = nullptr;

        ObjectEditor* editor = nullptr;

        Array<ClassType*> settingsClasses;

        f32 splitRatio = 0.2f;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ProjectSettingsEditor.rtti.h"
