#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORCORE_API ArrayPropertyEditor : public PropertyEditor
    {
        CE_CLASS(ArrayPropertyEditor, PropertyEditor)
    protected:

        ArrayPropertyEditor();

        void ConstructEditor() override;

    public: // - Public API -


    protected: // - Internal -


    public: // - Fusion Properties - 


        FUSION_WIDGET;
    };
    
}

#include "ArrayPropertyEditor.rtti.h"
