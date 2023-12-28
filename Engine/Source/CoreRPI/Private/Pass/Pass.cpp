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

    }

} // namespace CE::RPI
