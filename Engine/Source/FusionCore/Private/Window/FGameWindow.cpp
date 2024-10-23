#include "FusionCore.h"

namespace CE
{

    FGameWindow::FGameWindow()
    {

    }

    void FGameWindow::Construct()
    {
        Super::Construct();
        
    }

    void FGameWindow::OnBeforeDestroy()
    {
	    Super::OnBeforeDestroy();

        if (!IsDefaultInstance())
        {
            FusionApplication::Get()->OnRenderViewportDestroyed(this);
        }
    }

    void FGameWindow::SetScene(RPI::Scene* scene)
    {
        this->scene = scene;
    }

    bool FGameWindow::IsEmbeddedViewport() const
    {
        return IsOfType<FViewport>();
    }

}
