#pragma once

namespace CE
{
    CLASS()
    class EDITORCORE_API EditorDockspaceStyle : public FStyle
    {
        CE_CLASS(EditorDockspaceStyle, FStyle)
    public:

        virtual ~EditorDockspaceStyle();

    protected:

        EditorDockspaceStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

    public:

        FIELD()
		FBrush background = Color::RGBA(26, 26, 26);

		FIELD()
		Color borderColor = Color::RGBA(15, 15, 15);

		FIELD()
		f32 borderWidth = 1.0f;

		FIELD()
		FBrush titleBarBackground = Color::RGBA(26, 26, 26);

        FIELD()
        FBrush activeTabBackground = Color::RGBA(36, 36, 36);

        FIELD()
        FBrush inactiveTabBackground = Color::RGBA(26, 26, 26);

        FIELD()
        FBrush hoveredTabBackground = Color::RGBA(32, 32, 32);

        FIELD()
        FBrush projectLabelBackground = Color::RGBA(16, 16, 16);

        FIELD()
        Vec4 tabCornerRadius = Vec4(5, 5, 0, 0);

        FIELD()
        Color activeTabBorder = Color::Clear();

        FIELD()
        Color inactiveTabBorder = Color::Clear();

        FIELD()
        Color hoveredTabBorder = Color::Clear();

        FIELD()
        f32 tabBorderWidth = 0;

    };
    
} // namespace CE

#include "EditorDockspaceStyle.rtti.h"
