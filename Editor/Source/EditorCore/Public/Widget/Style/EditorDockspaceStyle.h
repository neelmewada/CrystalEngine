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
		FBrush background = Color::RGBA(36, 36, 36);

		FIELD()
		Color borderColor = Color::RGBA(15, 15, 15);

		FIELD()
		f32 borderWidth = 1.0f;

		FIELD()
		FBrush titleBarBackground = Color::RGBA(26, 26, 26);

    };
    
} // namespace CE

#include "EditorDockspaceStyle.rtti.h"
