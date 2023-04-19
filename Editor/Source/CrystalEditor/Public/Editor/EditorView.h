#pragma once

#include "Object/Object.h"

#include <QWidget>

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API EditorView : public QWidget, public CE::Object
    {
        Q_OBJECT
        CE_CLASS(EditorView, CE::Object)
    public:
        explicit EditorView(QWidget* parent = nullptr);
        virtual ~EditorView();

        void paintEvent(QPaintEvent *event) override;
        
    };
    
} // namespace CE::Editor

#include "EditorView.rtti.h"
