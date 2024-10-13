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

        CE::Scene* scene = nullptr;

    public: // - Fusion Properties -


        FUSION_WIDGET;
    };
    
}

#include "EditorViewport.rtti.h"
