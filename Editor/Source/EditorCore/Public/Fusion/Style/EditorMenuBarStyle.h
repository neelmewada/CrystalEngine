#pragma once

namespace CE
{
    CLASS()
    class EDITORCORE_API EditorMenuBarStyle : public FMenuBarStyle
    {
        CE_CLASS(EditorMenuBarStyle, FMenuBarStyle)
    public:

        virtual ~EditorMenuBarStyle();

    protected:

        EditorMenuBarStyle();

    public:

        SubClass<FWidget> GetWidgetClass() const override;

        void MakeStyle(FWidget& widget) override;

    };
    
} // namespace CE

#include "EditorMenuBarStyle.rtti.h"
