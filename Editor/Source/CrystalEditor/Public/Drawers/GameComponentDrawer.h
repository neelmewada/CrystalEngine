#pragma once

#include "CoreMinimal.h"

#include "QtComponents/Widgets/CardWidget.h"

#include "Drawers/DrawerBase.h"

namespace CE::Editor
{

    class FieldDrawer;
    class StringFieldDrawer;

    class CRYSTALEDITOR_API GameComponentDrawer : public DrawerBase, public IObjectUpdateListener<GameComponent>
    {
        CE_CLASS(GameComponentDrawer, DrawerBase)
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
        virtual TypeInfo* GetTargetType() override;
        virtual void* GetTargetInstance() override;

        virtual bool IsValid() override { return componentType != nullptr && targetComponent != nullptr; }

        virtual void OnObjectUpdated(GameComponent* component) override;

    private slots:
        void HandleCardContextMenu(const QPoint& pos);

    protected:

        ClassType* componentType = nullptr;
        GameComponent* targetComponent = nullptr;
        
        StringFieldDrawer* tempDrawer = nullptr;

        Array<FieldDrawer*> fieldDrawers{};
    };

}

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, GameComponentDrawer,
    CE_SUPER(CE::Editor::DrawerBase),
    CE_NOT_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
