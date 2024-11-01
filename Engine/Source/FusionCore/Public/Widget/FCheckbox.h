#pragma once

namespace CE
{
    CLASS()
    class FUSIONCORE_API FCheckbox : public FButton
    {
        CE_CLASS(FCheckbox, FButton)
    protected:

        FCheckbox();

        void Construct() override;

        void OnClick() override;

        void OnFusionPropertyModified(const CE::Name& propertyName) override;

    public: // - Public API -

        FImage* GetCheckmark() const { return checkmark; }

    protected:

        FImage* checkmark = nullptr;

    public: // - Fusion Properties - 

        FUSION_PROPERTY(bool, Checked);

        FUSION_EVENT(ScriptEvent<void(FCheckbox*)>, OnCheckChanged);

        FUSION_WIDGET;
    };
    
}

#include "FCheckbox.rtti.h"
