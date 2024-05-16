#pragma once

namespace CE::Editor
{
    CLASS()
    class EDITORSYSTEM_API PropertyEditorSection : public CCollapsibleSection
    {
        CE_CLASS(PropertyEditorSection, CCollapsibleSection)
    public:

        PropertyEditorSection();

        virtual ~PropertyEditorSection();

    protected:

        void Construct() override;

    private:

        
    };
    
} // namespace CE::Editor

#include "PropertyEditorSection.rtti.h"