#include "EditorCore.h"

namespace CE::Editor
{

    DetailsView::DetailsView()
    {

    }

    void DetailsView::Construct()
    {
        Super::Construct();

        // TODO: Create editors and bind them to respective fields of Objects

        // Editor widget will hold reference to FieldType* (edited field) and Array<Object*> (array of target objects)
        // Editor widget will listen to changes made in any of the Array<Object*> through code (NOT UI input) using the Setters.

    }
    
}

