#include "EditorCore.h"

namespace CE::Editor
{

    EditorBase::EditorBase()
    {

    }

    void EditorBase::Construct()
    {
	    Super::Construct();

        history = CreateObject<EditorHistory>(this, "EditorHistory");

    }

}

