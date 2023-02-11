
#include "QtComponents/StyleManager.h"

#include <QPalette>
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

        QColor darkGray(53, 53, 53);
        QColor gray(128, 128, 128);
        QColor black(25, 25, 25);
        QColor blue(42, 130, 218);

        // ezEditor styling
        palette.setColor(QPalette::WindowText, QColor(200, 200, 200, 255));
        palette.setColor(QPalette::Button, QColor(100, 100, 100, 255));
        palette.setColor(QPalette::Light, QColor(97, 97, 97, 255));
        palette.setColor(QPalette::Midlight, QColor(59, 59, 59, 255));
        palette.setColor(QPalette::Dark, QColor(37, 37, 37, 255));
        palette.setColor(QPalette::Mid, QColor(45, 45, 45, 255));
        palette.setColor(QPalette::Text, QColor(200, 200, 200, 255));
        palette.setColor(QPalette::BrightText, QColor(37, 37, 37, 255));
        palette.setColor(QPalette::ButtonText, QColor(200, 200, 200, 255));
        palette.setColor(QPalette::Base, QColor(42, 42, 42, 255));
        palette.setColor(QPalette::Window, QColor(68, 68, 68, 255));
        palette.setColor(QPalette::Shadow, QColor(0, 0, 0, 255));
        palette.setColor(QPalette::Highlight, QColor(177, 135, 27, 255));
        palette.setColor(QPalette::HighlightedText, QColor(255, 255, 255, 255));
        palette.setColor(QPalette::Link, QColor(0, 148, 255, 255));
        palette.setColor(QPalette::LinkVisited, QColor(255, 0, 220, 255));
        palette.setColor(QPalette::AlternateBase, QColor(46, 46, 46, 255));
        QBrush NoRoleBrush(QColor(0, 0, 0, 255), ::Qt::NoBrush);
        palette.setBrush(QPalette::NoRole, NoRoleBrush);
        palette.setColor(QPalette::ToolTipBase, QColor(255, 255, 220, 255));
        palette.setColor(QPalette::ToolTipText, QColor(0, 0, 0, 255));

        palette.setColor(QPalette::Disabled, QPalette::WindowText, QColor(128, 128, 128, 255));
        palette.setColor(QPalette::Disabled, QPalette::Button, QColor(80, 80, 80, 255));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(105, 105, 105, 255));
        palette.setColor(QPalette::Disabled, QPalette::BrightText, QColor(255, 255, 255, 255));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(128, 128, 128, 255));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(86, 117, 148, 255));

        // purple highlight theme override
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

        qApp->setStyle(QStyleFactory::create("Fusion"));

        // New Theme
        palette.setColor(QPalette::Window, QColor(53, 53, 53));
        palette.setColor(QPalette::WindowText, ::Qt::white);
        palette.setColor(QPalette::Base, QColor(25, 25, 25));
        palette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
        palette.setColor(QPalette::ToolTipBase, ::Qt::white);
        palette.setColor(QPalette::ToolTipText, ::Qt::white);
        palette.setColor(QPalette::Text, ::Qt::white);
        palette.setColor(QPalette::Button, QColor(53, 53, 53));
        palette.setColor(QPalette::ButtonText, ::Qt::white);
        palette.setColor(QPalette::BrightText, ::Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, ::Qt::black);

        palette.setColor(QPalette::Active, QPalette::Button, gray.darker());
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, gray);
        palette.setColor(QPalette::Disabled, QPalette::Text, gray);
        palette.setColor(QPalette::Disabled, QPalette::Light, darkGray);

        qApp->setStyleSheet("QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }");

        app->setPalette(palette);
    }

} // namespace CE::Editor::Qt
