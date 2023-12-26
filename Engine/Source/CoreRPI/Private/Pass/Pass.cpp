#include "CoreRPI.h"

namespace CE::RPI
{

	Pass::~Pass()
	{
		
	}

    Pass::Pass()
    {
		if (drawListTag.IsValid())
		{
			RPISystem::Get().GetDrawListTagRegistry()->ReleaseTag(drawListTag);
		}

		for (int i = 0; i < attachmentBindings.GetSize(); i++)
		{
			attachmentBindings[i].ownerPass = this;
		}
    }

} // namespace CE::RPI
