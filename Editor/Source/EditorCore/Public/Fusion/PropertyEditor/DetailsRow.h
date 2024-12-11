#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API DetailsRow : public FButton
    {
        CE_CLASS(DetailsRow, FButton)
    protected:

        DetailsRow();

        void Construct() override;

    public: // - Public API -

        bool SupportsKeyboardFocus() const override { return false; }

    protected: // - Internal -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "DetailsRow.rtti.h"
