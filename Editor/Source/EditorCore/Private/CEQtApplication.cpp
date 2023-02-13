
#include "CEQtApplication.h"

namespace CE::Editor::Qt
{

    CEQtApplication::CEQtApplication(int argc, char** argv)
        : QApplication(argc, argv)
        , argc(argc), argv(argv)
    {
        // Use a common Qt settings path for applications that don't register their own application name
        QApplication::setOrganizationName("CE");
        QApplication::setApplicationName("CE Tools Application");

        StyleManager.Initialize(this);

        InitializeDpiScaling();
    }

    CEQtApplication::~CEQtApplication()
    {

    }

    void CEQtApplication::InitializeDpiScaling()
    {
        QCoreApplication::setAttribute(::Qt::AA_EnableHighDpiScaling);
        QCoreApplication::setAttribute(::Qt::AA_UseHighDpiPixmaps);
        QCoreApplication::setAttribute(::Qt::AA_DontCreateNativeWidgetSiblings);
        QGuiApplication::setHighDpiScaleFactorRoundingPolicy(::Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
    }

} // namespace CE::Editor

CE_RTTI_CLASS_IMPL(EDITORCORE_API, CE::Editor::Qt, CEQtApplication)
