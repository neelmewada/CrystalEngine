#pragma once

namespace CE::Editor
{
    ENUM()
    enum class EditorDockTabRole
    {
	    Default,
        Minor
    };
    ENUM_CLASS(EditorDockTabRole);

    CLASS()
    class EDITORCORE_API EditorDockTab : public EditorWindow
    {
        CE_CLASS(EditorDockTab, EditorWindow)
    public:

        // - Public API -

        virtual bool CanBeClosed() const { return true; }

    protected:

        EditorDockTab();

        void Construct() override;

        void OnBeginDestroy() override;

        EditorMenuBar* menuBar = nullptr;
        EditorToolBar* toolBar = nullptr;
        FStyledWidget* content = nullptr;

        FWidget* dockspace = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(String, Title);

        FUSION_PROPERTY(EditorDockTabRole, TabRole);

        FUSION_PROPERTY_WRAPPER2(Enabled, toolBar, ToolBarEnabled);
        FUSION_PROPERTY_WRAPPER2(Enabled, menuBar, MenuBarEnabled);

        FUSION_PROPERTY_WRAPPER2(Background, content, ContentBackground);
        FUSION_PROPERTY_WRAPPER2(BackgroundShape, content, ContentBackgroundShape);
        FUSION_PROPERTY_WRAPPER2(BorderColor, content, ContentBorderColor);
        FUSION_PROPERTY_WRAPPER2(BorderWidth, content, ContentBorderWidth);

        Self& Content(FWidget& widget);

        FUSION_WIDGET;
        friend class EditorDockspace;
        friend class EditorMinorDockspace;
        friend class EditorDockTabItem;
    };

    CLASS()
    class EDITORCORE_API EditorMinorDockTab : public EditorDockTab
    {
        CE_CLASS(EditorMinorDockTab, EditorDockTab)
    protected:

        EditorMinorDockTab();

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

        void Construct() override;

        friend class EditorDockspace;
        friend class EditorMinorDockspace;
        friend class EditorDockTabItem;
    };
    
}

#include "EditorDockTab.rtti.h"
