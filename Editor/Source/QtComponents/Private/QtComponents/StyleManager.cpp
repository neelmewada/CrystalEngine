
#include "QtComponents/StyleManager.h"

#include <QPalette>
#include <QStyleFactory>
#include <QPointer>
#include <QStyle>
#include <QMenu>

const char* globalStyleSheetText = R"(
QToolTip { color: #ffffff; background-color: #2a82da; border: 1px solid white; }
ads--CDockWidgetTab {
    background: palette(window);
    border-top-left-radius: 5px;
    border-top-right-radius: 5px;
    border: 0px;
    padding: 3px 0px;
}
ads--CDockWidgetTab[activeTab="true"] {
    background: palette(alternate-base);
}
ads--CDockWidgetTab[activeTab="true"] > #tabCloseButton {
    qproperty-icon: url(:/Editor/Icons/close);
}
ads--CDockWidget {
    border: 0px;
}
ads--CDockWidget > QWidget {
    background: palette(alternate-base);
    border: 0px;
}
EditorViewBase {
    background: palette(alternate-base);
    border: 0px;
}
#dockAreaCloseButton {
    icon: url(:/Editor/Icons/close);
}
ads--CDockWidgetTab QLabel {
    min-height: 16px;
    min-width: 40px;
    color: white;
}
QMainWindow > QToolBar {
    padding: 0px 5px;
    background-color: rgb(36, 36, 36);
    border: 0px;
    border-bottom: 2px solid palette(base);
}
QToolBar > QToolButton {
    border: 0px;
    border-radius: 3px;
    padding: 3px 3px;
}
QToolBar > QToolButton:hover {
    background: rgb(60, 60, 60);
}
QToolBar > QToolButton:pressed {
    background: rgb(80, 80, 80);
}
QToolBar > QToolButton:checked {
    background: rgb(80, 80, 80);
}
QMenu {
    background-color: rgb(56, 56, 56);
    border: 1px solid rgb(80, 80, 80);
}
QMenu::item:selected {
    color: white;
    background-color: rgb(0, 112, 224);
}
)";

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

        QColor darkGray(36, 36, 36); // Color!
        QColor gray(128, 128, 128);
        QColor black(26, 26, 26);
        QColor blue(42, 130, 218);

        // ezEditor styling
        palette.setColor(QPalette::WindowText, QColor(200, 200, 200, 255));
        palette.setColor(QPalette::Button, QColor(60, 60, 60, 255));
        palette.setColor(QPalette::Light, QColor(97, 97, 97, 255));
        //palette.setColor(QPalette::Light, QColor(37, 37, 37, 255));
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
        palette.setColor(QPalette::Disabled, QPalette::Button, QColor(32, 32, 32, 255));
        palette.setColor(QPalette::Disabled, QPalette::Text, QColor(105, 105, 105, 255));
        palette.setColor(QPalette::Disabled, QPalette::BrightText, QColor(255, 255, 255, 255));
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, QColor(128, 128, 128, 255));
        palette.setColor(QPalette::Disabled, QPalette::Highlight, QColor(86, 117, 148, 255));
        
        // purple highlight theme override
        palette.setColor(QPalette::Window, darkGray);
        palette.setColor(QPalette::WindowText, ::Qt::white);
        palette.setColor(QPalette::AlternateBase, darkGray);
        palette.setColor(QPalette::ToolTipBase, ::Qt::white);
        palette.setColor(QPalette::ToolTipText, ::Qt::white);
        palette.setColor(QPalette::Text, ::Qt::white);
        palette.setColor(QPalette::Button, darkGray);
        palette.setColor(QPalette::ButtonText, ::Qt::white);
        palette.setColor(QPalette::BrightText, ::Qt::red);

        palette.setColor(QPalette::Highlight, QColor(142, 45, 197).lighter());
        palette.setColor(QPalette::HighlightedText, ::Qt::black);

        // New Theme
        palette.setColor(QPalette::Window, QColor(21, 21, 21));
        palette.setColor(QPalette::WindowText, ::Qt::white);
        palette.setColor(QPalette::Base, black);
        palette.setColor(QPalette::AlternateBase, darkGray);
        palette.setColor(QPalette::ToolTipBase, ::Qt::white);
        palette.setColor(QPalette::ToolTipText, ::Qt::white);
        palette.setColor(QPalette::Text, ::Qt::white);
        palette.setColor(QPalette::Button, darkGray);
        palette.setColor(QPalette::ButtonText, ::Qt::white);
        palette.setColor(QPalette::BrightText, ::Qt::red);
        palette.setColor(QPalette::Link, QColor(42, 130, 218));
        palette.setColor(QPalette::Highlight, QColor(42, 130, 218));
        palette.setColor(QPalette::HighlightedText, ::Qt::black);

        palette.setColor(QPalette::PlaceholderText, gray);
        palette.setColor(QPalette::Active, QPalette::Button, darkGray);
        palette.setColor(QPalette::Disabled, QPalette::ButtonText, gray);
        palette.setColor(QPalette::Disabled, QPalette::WindowText, gray);
        palette.setColor(QPalette::Disabled, QPalette::Text, gray);
        palette.setColor(QPalette::Disabled, QPalette::Light, darkGray);

        app->setStyleSheet(globalStyleSheetText);

        app->setPalette(palette);
    }

} // namespace CE::Editor::Qt
