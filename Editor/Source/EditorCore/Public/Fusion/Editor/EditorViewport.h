#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API EditorViewport : public FViewport
    {
        CE_CLASS(EditorViewport, FViewport)
    public:

        // - Public API -

        

    protected:

        EditorViewport();

        void Construct() override;

    public: // - Fusion Properties -


        FUSION_WIDGET;
    };
    
}

#include "EditorViewport.rtti.h"
