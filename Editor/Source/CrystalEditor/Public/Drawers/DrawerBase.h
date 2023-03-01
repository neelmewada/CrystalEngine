#pragma once

#include "CoreMinimal.h"

#include <QWidget>
#include <QLayout>

#define CE_REGISTER_DRAWER(drawerClass, targetType) CE::Editor::DrawerBase::RegisterDrawer(TYPEID(targetType), drawerClass::Type())
#define CE_REGISTER_DRAWERS(drawerClass, ...) CE::Editor::DrawerBase::RegisterDrawers<__VA_ARGS__>(drawerClass::Type())

namespace CE::Editor
{
    
    class CRYSTALEDITOR_API DrawerBase : public CE::Object
    {
        CE_CLASS(DrawerBase, CE::Object)
    protected:
        DrawerBase(CE::Name name);
    
    public:
        virtual ~DrawerBase();

        static void RegisterDrawer(TypeId targetType, ClassType* drawerClass);
        static void RegisterDrawer(Array<TypeId> targetTypes, ClassType* drawerClass);

        template<typename... Args>
        static void RegisterDrawers(ClassType* drawerClass)
        {
            Array<TypeId> targetTypes = { TYPEID(Args)... };
            RegisterDrawer(targetTypes, drawerClass);
        }

        template<>
        static void RegisterDrawers(ClassType* drawerClass) {}

        static ClassType* GetDrawerClassFor(TypeId targetType);

    public: // API

        virtual void OnEnable() {}
        virtual void OnDisable() {}

        virtual void CreateGUI(QLayout* container) = 0;
        virtual void ClearGUI(QLayout* container) = 0;

        virtual void SetTarget(TypeInfo* targetType, void* instance) = 0;
        virtual TypeInfo* GetTargetType() = 0;
        virtual void* GetTargetInstance() = 0;

        virtual bool IsValid() = 0;
        
    protected:
        static CE::HashMap<TypeId, ClassType*> drawerClassMap;
    };

} // namespace CE::Editor

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, DrawerBase,
    CE_SUPER(CE::Object),
    CE_ABSTRACT,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
