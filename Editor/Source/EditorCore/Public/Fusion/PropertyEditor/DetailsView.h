#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API DetailsView : public FStyledWidget
    {
        CE_CLASS(DetailsView, FStyledWidget)
    protected:

        DetailsView();

        void Construct() override;

    public: // - Public API -

    public: // - Fusion Properties - 


    protected:

        Array<Object*> targets;
        Object* target = nullptr;

        FUSION_WIDGET;
    };
    
}

#include "DetailsView.rtti.h"
