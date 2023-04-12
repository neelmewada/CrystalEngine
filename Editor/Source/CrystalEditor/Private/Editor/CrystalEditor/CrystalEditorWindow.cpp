#include "CrystalEditorWindow.h"
#include "ui_CrystalEditorWindow.h"

#include "Editor/SceneEditor/SceneEditorWindow.h"

#include <QLabel>

namespace CE::Editor
{
    HashMap<BuiltinAssetType, ClassType*> CrystalEditorBus::Interface::builtinAssetEditors{};
    HashMap<TypeId, ClassType*> CrystalEditorBus::Interface::assetEditors{};

    CrystalEditorWindow::CrystalEditorWindow(QWidget* parent)
        : QMainWindow(parent)
        , CE::Object("CrystalEditorWindow")
        , ui(new Ui::CrystalEditorWindow)
    {
        CE_CONNECT(CrystalEditorBus, this);

        ui->setupUi(this);

        setWindowTitle("Crystal Editor");

        setMinimumWidth(1280);
        setMinimumHeight(720);

        auto signal = AssetManager::Type()->FindFunctionWithName("OnAssetUpdated");
        auto event = Self::Type()->FindFunctionWithName("OnAssetUpdated");
        if (signal != nullptr && event != nullptr)
        {
            Object::Bind(&AssetManager::Get(), signal, this, event);
        }

        using namespace ads;

        CDockManager::setConfigFlag(CDockManager::DockAreaHasUndockButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasTabsMenuButton, false);
        CDockManager::setConfigFlag(CDockManager::DockAreaHasCloseButton, false);

        mainDockManager = new ads::CDockManager(this);

        // Scene Editor Window
        auto sceneEditor = new SceneEditorWindow();
        auto dockWidget = new ads::CDockWidget(sceneEditor->windowTitle());
        dockWidget->setWidget(sceneEditor);
        editorWindows.Add(sceneEditor);
        dockWidgets.Add(dockWidget);
        
        mainDockManager->addDockWidget(ads::TopDockWidgetArea, dockWidget);
    }

    CrystalEditorWindow::~CrystalEditorWindow()
    {
        CE_DISCONNECT(CrystalEditorBus, this);

        delete ui;
    }

    void CrystalEditorWindow::OnAssetUpdated(IO::Path assetPath)
    {
        CE_LOG(Info, All, "OnAssetUpdated: {}", assetPath);
    }

    void CrystalEditorBus::Interface::RegisterEditorWindowClassForBuiltinAsset(ClassType* editorWindowClass, BuiltinAssetType builtinAssetType)
    {
        if (builtinAssetType == BuiltinAssetType::None || editorWindowClass == nullptr ||
            !editorWindowClass->IsAssignableTo(TYPEID(EditorWindowBase)) ||
            editorWindowClass->GetTypeId() == TYPEID(EditorWindowBase))
            return;

        builtinAssetEditors[builtinAssetType] = editorWindowClass;
    }

    void CrystalEditorBus::Interface::RegisterEditorWindowClassForAsset(ClassType* editorWindowClass, ClassType* assetClass)
    {
        if (editorWindowClass == nullptr || assetClass == nullptr ||
            !editorWindowClass->IsAssignableTo(TYPEID(EditorWindowBase)) ||
            editorWindowClass->GetTypeId() == TYPEID(EditorWindowBase) ||
            !assetClass->IsAssignableTo(TYPEID(Asset)) ||
            assetClass->GetTypeId() == TYPEID(Asset))
            return;

        assetEditors[assetClass->GetTypeId()] = editorWindowClass;
    }

    ClassType* CrystalEditorBus::Interface::GetBuiltinAssetEditorWindow(BuiltinAssetType builtinAssetType)
    {
        if (!builtinAssetEditors.KeyExists(builtinAssetType))
            return nullptr;
        return builtinAssetEditors[builtinAssetType];
    }

    ClassType* CrystalEditorBus::Interface::GetAssetEditorWindow(TypeId assetClassType)
    {
        if (!assetEditors.KeyExists(assetClassType))
            return nullptr;
        return assetEditors[assetClassType];
    }

    ClassType* CrystalEditorBus::Interface::GetAssetEditorWindow(ClassType* assetClass)
    {
        if (assetClass == nullptr || !assetEditors.KeyExists(assetClass->GetTypeId()))
            return nullptr;
        return assetEditors[assetClass->GetTypeId()];
    }

    void CrystalEditorWindow::LoadProject(IO::Path projectPath)
    {
        
    }

    void CrystalEditorWindow::OpenAsset(AssetDatabaseEntry* assetEntry)
    {
        if (assetEntry == nullptr || assetEntry->entryType != AssetDatabaseEntry::Type::Asset)
            return;

        auto basePath = ProjectSettings::Get().GetEditorProjectDirectory();
        if (assetEntry->category == AssetDatabaseEntry::Category::EngineAssets || assetEntry->category == AssetDatabaseEntry::Category::EngineShaders)
        {
            basePath = PlatformDirectories::GetEngineDir().GetParentPath();
        }

        auto assetPath = basePath / assetEntry->GetVirtualPath();
        if (!assetPath.Exists())
        {
            CE_LOG(Error, All, "Failed to open asset! Path doesn't exist: {}", assetPath);
            return;
        }

        auto builtinAssetType = Asset::GetBuiltinAssetTypeFor(assetPath.GetExtension().GetString());

        if (builtinAssetType != BuiltinAssetType::None)
        {
            auto editorWindowClass = GetBuiltinAssetEditorWindow(builtinAssetType);
            if (editorWindowClass == nullptr)
                return;

            bool windowFound = false;

            for (auto editorWindow: editorWindows)
            {
                if (editorWindow->GetType()->GetTypeId() == editorWindowClass->GetTypeId())
                {
                    windowFound = true;
                    if (editorWindow->OpenAsset(assetEntry))
                    {
                        return;
                    }
                }
            }

            if (!windowFound && editorWindowClass->CanBeInstantiated())
            {
                auto editorWindowClassInstance = (EditorWindowBase*)editorWindowClass->CreateDefaultInstance();

                if (editorWindowClassInstance->CanOpenAsset(assetEntry))
                {
                    auto dockWidget = new ads::CDockWidget(editorWindowClassInstance->windowTitle());
                    dockWidget->setWidget(editorWindowClassInstance);
                    editorWindows.Add(editorWindowClassInstance);
                    dockWidgets.Add(dockWidget);

                    mainDockManager->addDockWidgetTab(ads::TopDockWidgetArea, dockWidget);

                    if (!editorWindowClassInstance->OpenAsset(assetEntry))
                    {
                        // Failed to open asset in editor
                        mainDockManager->removeDockWidget(dockWidget);

                        dockWidgets.Remove(dockWidget);
                        editorWindows.Remove(editorWindowClassInstance);

                        editorWindowClassInstance->close();

                        editorWindowClass->DestroyInstance(editorWindowClassInstance);
                        editorWindowClassInstance = nullptr;
                        CE_LOG(Error, All, "Failed to open asset at path: {}", assetPath);
                        return;
                    }
                }
                else
                {
                    editorWindowClass->DestroyInstance(editorWindowClassInstance);
                    editorWindowClassInstance = nullptr;
                    CE_LOG(Error, All, "Failed to open asset at path: {}", assetPath);
                    return;
                }
            }
        }
    }

}
