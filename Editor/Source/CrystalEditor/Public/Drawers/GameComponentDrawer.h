#pragma once

#include "CoreMinimal.h"

#include "QtComponents/Widgets/CardWidget.h"

#include "Drawers/DrawerBase.h"

namespace CE::Editor
{

    class FieldDrawer;
    class StringFieldDrawer;

    CLASS()
    class CRYSTALEDITOR_API GameComponentDrawer : public DrawerBase, public IObjectUpdateListener<GameComponent>
    {
        CE_CLASS(GameComponentDrawer, CE::Editor::DrawerBase)
    protected:
        GameComponentDrawer();
    public:
        virtual ~GameComponentDrawer();

        static ClassType* GetGameComponentDrawerClassFor(TypeId targetType);

    public: // API

        virtual void OnEnable() override;
        virtual void OnDisable() override;

        virtual void CreateGUI(QLayout* container) override;
        virtual void ClearGUI(QLayout* container) override;

        virtual void SetTarget(TypeInfo* targetType, void* instance) override;
        virtual void SetTargets(TypeInfo* targetType, Array<void*> instances) override;
        virtual TypeInfo* GetTargetType() override;
        virtual void* GetTargetInstance() override;

        virtual bool IsValid() override { return componentType != nullptr && targetComponent != nullptr; }

        virtual void OnObjectUpdated(GameComponent* component) override;

    private slots:
        void HandleCardContextMenu(const QPoint& pos);

    protected:

        ClassType* componentType = nullptr;
        GameComponent* targetComponent = nullptr;
        Array<GameComponent*> multipleTargetComponents{};

        StringFieldDrawer* tempDrawer = nullptr;

        Array<FieldDrawer*> fieldDrawers{};
    };

}

#include "GameComponentDrawer.rtti.h"

