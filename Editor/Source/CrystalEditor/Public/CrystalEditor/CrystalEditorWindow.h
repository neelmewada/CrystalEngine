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

        void Construct() override;
        
    };
    
} // namespace CE::Editor

#include "CrystalEditorWindow.rtti.h"