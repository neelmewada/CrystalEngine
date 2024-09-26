#include "Fusion.h"

namespace CE
{

    FListViewModel::FListViewModel()
    {

    }

    FListViewModel::~FListViewModel()
    {
        
    }

    void FListViewModel::OnModelPropertyEdited(const CE::Name& propertyName, Object* modifyingObject)
    {
	    Super::OnModelPropertyEdited(propertyName, modifyingObject);

    }

} // namespace CE

