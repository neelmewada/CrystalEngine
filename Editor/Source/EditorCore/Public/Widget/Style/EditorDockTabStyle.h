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

    };
    
} // namespace CE

#include "EditorDockTabStyle.rtti.h"
