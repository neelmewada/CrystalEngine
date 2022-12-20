#pragma once

#include <QObject>
#include <QApplication>

#include <string>
#include <cstring>

class QApplication;
class QStyle;
class QWidget;

namespace CE::Editor::Qt
{
    
    class QTCOMPONENTS_API StyleManager
        : public QObject
    {
        Q_OBJECT
        
    public:

        explicit StyleManager(QObject* parent);
        ~StyleManager() override;

        void Initialize(QApplication* app);

    private Q_SLOTS:


    };

} // namespace CE::Editor::Qt
