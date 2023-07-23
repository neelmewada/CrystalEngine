#pragma once

namespace CE::Widgets
{

    CLASS()
    class COREWIDGETS_API CSpacer : public CWidget
    {
        CE_CLASS(CSpacer, CWidget)
    public:

        CSpacer();

        virtual ~CSpacer();

		virtual void Construct() override;

		virtual void OnBeforeComputeStyle() override;

		inline f32 GetExpandFactor() const { return expandFactor; }
		inline void SetExpandFactor(f32 value) { expandFactor = value; }
		
		inline f32 GetShrinkFactor() const { return shrinkFactor; }
		inline void SetShrinkFactor(f32 value) { shrinkFactor = value; }

    protected:

        virtual void OnDrawGUI() override;


		FIELD()
		f32 expandFactor = 1;

		FIELD()
		f32 shrinkFactor = 3;
    };
    
} // namespace CE

#include "CSpacer.rtti.h"
