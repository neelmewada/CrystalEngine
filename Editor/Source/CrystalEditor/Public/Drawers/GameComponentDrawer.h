#pragma once

#include "CoreMinimal.h"

#include "DrawerBase.h"

namespace CE::Editor
{

    class CRYSTALEDITOR_API GameComponentDrawer : public DrawerBase
    {
        CE_CLASS(GameComponentDrawer, DrawerBase)
    public:

    };

}

CE_RTTI_CLASS(CRYSTALEDITOR_API, CE::Editor, GameComponentDrawer,
    CE_SUPER(CE::Editor::DrawerBase),
    CE_DONT_INSTANTIATE,
    CE_ATTRIBS(),
    CE_FIELD_LIST(),
    CE_FUNCTION_LIST()
)
