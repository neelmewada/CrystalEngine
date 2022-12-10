
#include "CEQtApplication.h"

#include <QStyleFactory>
#include <QPalette>

namespace CE::Editor::Qt
{

    CEQtApplication::CEQtApplication(int argc, char** argv) : QApplication(argc, argv)
    {
        // Use a common Qt settings path for applications that don't register their own application name
        QApplication::setOrganizationName("CE");
        QApplication::setApplicationName("CE Tools Application");

        QApplication::setStyle(QStyleFactory::create("Fusion"));

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
        qApp->setPalette(palette);
    }

    void CEQtApplication::InitializeDpiScaling()
    {
        QCoreApplication::setAttribute(::Qt::AA_EnableHighDpiScaling);
        QCoreApplication::setAttribute(::Qt::AA_UseHighDpiPixmaps);
        QCoreApplication::setAttribute(::Qt::AA_DontCreateNativeWidgetSiblings);
        QGuiApplication::setHighDpiScaleFactorRoundingPolicy(::Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    }
} // namespace CE::Editor
