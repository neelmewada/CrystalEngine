#pragma once

#include "Object/Object.h"

#include <QWidget>

namespace CE::Editor
{
    CLASS()
    class EditorViewBase : public QWidget, public CE::Object
    {
        Q_OBJECT
        CE_CLASS(EditorViewBase, CE::Object)
    public:
        explicit EditorViewBase(QWidget* parent = nullptr);
        virtual ~EditorViewBase();

        void paintEvent(QPaintEvent *event) override;
        
    };
    
} // namespace CE::Editor

#include "EditorViewBase.rtti.h"
