#pragma once

namespace CE
{
    CLASS()
    class EDITORCORE_API EditorDockTabStyle : public FStyle
    {
        CE_CLASS(EditorDockTabStyle, FStyle)
    public:

        virtual ~EditorDockTabStyle();

    protected:

        EditorDockTabStyle();

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

    public:

        FIELD()
        FBrush background = Color::RGBA(36, 36, 36);

        FIELD()
        Color borderColor = Color::Clear();

        FIELD()
        f32 borderWidth = 0.0f;

    };
    
} // namespace CE

#include "EditorDockTabStyle.rtti.h"
