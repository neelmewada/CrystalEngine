#include "CrystalEditor.h"

namespace CE::Editor
{

    EditorViewportTab::EditorViewportTab()
    {

    }

    void EditorViewportTab::Construct()
    {
        Super::Construct();

        (*this)
			.Title("Viewport")
            .Content(
                FAssignNew(EditorViewport, viewport)
                .HAlign(HAlign::Fill)
                .VAlign(VAlign::Fill)
            );
    }
    
}

