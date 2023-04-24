#pragma once

#include "Object/Object.h"
#include "Object/SystemObject.h"

namespace CE
{
    namespace Editor
    {
        class CrystalEditorApplication;
    }

    /// The class that manages the game engine and it's subsystems
    CLASS(Abstract)
    class SYSTEM_API Engine : public Object
    {
        CE_CLASS(Engine, CE::Object);
    private:
        Engine();
        virtual ~Engine();

    public:

        virtual void Startup();
        virtual void Shutdown();

        virtual void Tick(f32 deltaTime);

        //virtual void OnSystemComponentAdded(SystemComponent* component) override;
        //virtual void OnSystemComponentRemoved(SystemComponent* component) override;

        friend class SystemModule;
        friend class Application;
        friend class CE::Editor::CrystalEditorApplication;
    };

    SYSTEM_API extern Engine* gEngine;
    
} // namespace CE


#include "Engine.rtti.h"
