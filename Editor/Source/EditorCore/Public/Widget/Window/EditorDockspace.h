#pragma once

namespace CE::Editor
{
    class EditorDockTab;

    CLASS()
    class EDITORCORE_API EditorDockspace : public EditorWindow
    {
        CE_CLASS(EditorDockspace, EditorWindow)
    public:

        // - Public API -

    protected:

        EditorDockspace();

        void Construct() override;

        FStyledWidget* borderWidget = nullptr;
        FVerticalStack* rootBox = nullptr;
        FImage* maximizeIcon = nullptr;
        FImage* minimizeIcon = nullptr;
        FStackBox* content = nullptr;
        FTitleBar* titleBar = nullptr;
        FLabel* titleBarLabel = nullptr;

        FButton* minimizeButton = nullptr;
        FButton* maximizeButton = nullptr;

        FHorizontalStack* tabWell = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY_WRAPPER(Background, borderWidget);
        FUSION_PROPERTY_WRAPPER(BorderWidth, borderWidget);
        FUSION_PROPERTY_WRAPPER(BorderColor, borderWidget);

        FUSION_PROPERTY_WRAPPER2(Background, titleBar, TitleBarBackground);

        FUSION_WIDGET;
    };
    
}

#include "EditorDockspace.rtti.h"
