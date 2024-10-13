#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FGameWindow : public FWindow
    {
        CE_CLASS(FGameWindow, FWindow)
    public:

        // - Public API -

        void SetScene(RPI::Scene* scene);

        RPI::Scene* GetScene() const { return scene; }

        bool IsEmbeddedViewport() const;

    protected:

        FGameWindow();

        void Construct() override;

        void OnBeforeDestroy() override;

        RPI::Scene* scene = nullptr;

    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "FGameWindow.rtti.h"
