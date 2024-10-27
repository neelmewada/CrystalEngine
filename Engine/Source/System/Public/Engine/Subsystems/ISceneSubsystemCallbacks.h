#pragma once

namespace CE
{
    class Scene;

    struct ISceneSubsystemCallbacks
    {
        virtual ~ISceneSubsystemCallbacks() = default;

        virtual void OnSceneLoaded(CE::Scene* scene) {}

    };
    
} // namespace CE
