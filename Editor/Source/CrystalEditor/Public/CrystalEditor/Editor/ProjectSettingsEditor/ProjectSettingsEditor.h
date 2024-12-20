#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API ProjectSettingsEditor : public EditorBase
    {
        CE_CLASS(ProjectSettingsEditor, EditorBase)
    protected:

        ProjectSettingsEditor();

        void Construct() override;

        void OnBeginDestroy() override;

    public: // - Public API -

        static ProjectSettingsEditor* Show();

    protected: // - Internal -

        FUNCTION()
        void ShowAllSettings();

        void OnSettingsItemClicked(int index);

        FVerticalStack* left = nullptr;
        FVerticalStack* right = nullptr;

        ObjectEditor* editor = nullptr;

        Array<ObjectEditor*> editors;
        Array<ClassType*> settingsClasses;

        f32 splitRatio = 0.2f;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ProjectSettingsEditor.rtti.h"
