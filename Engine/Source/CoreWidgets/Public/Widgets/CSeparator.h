#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CSeparator : public CWidget
    {
        CE_CLASS(CSeparator, CWidget)
    public:

        CSeparator();

        virtual ~CSeparator();

		Vec2 CalculateIntrinsicContentSize(f32 width, f32 height) override;

		void OnAfterComputeStyle() override;

    protected:

        virtual void OnDrawGUI() override;

    };
    
} // namespace CE

#include "CSeparator.rtti.h"
