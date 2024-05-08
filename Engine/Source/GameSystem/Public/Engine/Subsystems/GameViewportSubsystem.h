#pragma once

namespace CE
{
    CLASS()
    class GAMESYSTEM_API GameViewportSubsystem : public EngineSubsystem
    {
        CE_CLASS(GameViewportSubsystem, EngineSubsystem)
    public:

        GameViewportSubsystem();
        ~GameViewportSubsystem();

        void Initialize() override;
        void PostInitialize() override;

        void PreShutdown() override;
        void Shutdown() override;

        f32 GetTickPriority() const override;

    private:

        CGameWindow* gameWindow = nullptr;

    };
    
} // namespace CE

#include "GameViewportSubsystem.rtti.h"