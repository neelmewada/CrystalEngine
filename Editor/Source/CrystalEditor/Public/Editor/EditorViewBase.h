#pragma once

#include <QWidget>

namespace CE::Editor
{

    class EditorViewBase : public QWidget
    {
        Q_OBJECT
        
    public:
        explicit EditorViewBase(QWidget* parent = nullptr);
        virtual ~EditorViewBase();
        
    };
    
} // namespace CE::Editor
