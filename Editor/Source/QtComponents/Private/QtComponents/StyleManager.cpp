
#include "QtComponents/StyleManager.h"

#include <QPalette>
#include <QStyleFactory>
#include <QStyleFactory>
#include <QPointer>
#include <QStyle>


namespace CE::Editor::Qt
{

    StyleManager::StyleManager(QObject* parent)
        : QObject(parent)
    {

    }

    StyleManager::~StyleManager()
    {

    }

    void StyleManager::Initialize(QApplication* app)
    {
        auto baseStyle = QStyleFactory::create("Fusion");

        QApplication::setStyle(baseStyle);

        QPalette palette;
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, ::Qt::white);
        palette.setColor(QPalette::Base, QColor(15, 15, 15));
        palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        palette.setColor(QPalette::ToolTipBase, ::Qt::white);
        palette.setColor(QPalette::ToolTipText, ::Qt::white);
        palette.setColor(QPalette::Text, ::Qt::white);
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, ::Qt::white);
        palette.setColor(QPalette::BrightText, ::Qt::red);

        palette.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
        palette.setColor(QPalette::HighlightedText, ::Qt::black);
        app->setPalette(palette);
    }

} // namespace CE::Editor::Qt
