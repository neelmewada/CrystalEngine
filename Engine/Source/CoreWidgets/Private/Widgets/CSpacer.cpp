#include "CoreWidgets.h"


namespace CE::Widgets
{

    CSpacer::CSpacer()
    {

    }

    CSpacer::~CSpacer()
    {
        
    }

	void CSpacer::Construct()
	{
		Super::Construct();
	}

	void CSpacer::OnBeforeComputeStyle()
	{
		Super::OnBeforeComputeStyle();
		
		YGNodeStyleSetFlexGrow(GetNode(), expandFactor);
		YGNodeStyleSetFlexShrink(GetNode(), shrinkFactor);
	}

    void CSpacer::OnDrawGUI()
    {
		auto rect = GetComputedLayoutRect();

		DrawDefaultBackground();
    }
    
} // namespace CE

