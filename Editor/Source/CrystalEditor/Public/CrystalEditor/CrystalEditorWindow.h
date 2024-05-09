#pragma once

namespace CE::Editor
{
    CLASS()
    class CRYSTALEDITOR_API CrystalEditorWindow final : public CDockSpace
    {
        CE_CLASS(CrystalEditorWindow, CDockSpace)
    public:

        CrystalEditorWindow();

        virtual ~CrystalEditorWindow();

    private:


        FUNCTION()
        void TimerTick();

        void Construct() override;

        void OnBeforeDestroy() override;

        FIELD()
        SceneEditorWindow* sceneEditor = nullptr;

        FIELD()
        ViewportWindow* viewportWindow = nullptr;

        SceneSubsystem* sceneSubsystem = nullptr;
        RendererSubsystem* rendererSubsystem = nullptr;

        // Sample fields

        StaticMeshComponent* meshComponent = nullptr;
        f32 rotation = 0;
        clock_t prevClock = 0;

    };
    
} // namespace CE::Editor

#include "CrystalEditorWindow.rtti.h"