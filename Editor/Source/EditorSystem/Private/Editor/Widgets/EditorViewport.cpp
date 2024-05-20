#include "EditorSystem.h"

namespace CE::Editor
{

    EditorViewport::EditorViewport()
    {
        allowHorizontalScroll = allowVerticalScroll = false;

        format = Format::R8G8B8A8_UNORM;
    }

    EditorViewport::~EditorViewport()
    {
	    
    }

    
} // namespace CE::Editor
