#include "FusionCore.h"

namespace CE
{

    FMenuPopup::FMenuPopup()
    {

    }

    void FMenuPopup::Construct()
    {
	    Super::Construct();

        Child(
            FNew(FVerticalStack)
        );
    }

}

