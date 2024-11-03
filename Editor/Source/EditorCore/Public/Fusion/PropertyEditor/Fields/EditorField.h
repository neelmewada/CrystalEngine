#pragma once

namespace CE::Editor
{
    CLASS(Abstract)
    class EDITORCORE_API EditorField : public FCompoundWidget
    {
        CE_CLASS(EditorField, FCompoundWidget)
    protected:

        EditorField();

        void Construct() override;

    public: // - Public API -


    protected: // - Internal -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "EditorField.rtti.h"
