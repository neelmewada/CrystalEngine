#include "CoreRPI.h"

namespace CE::RPI
{

	PassTree::~PassTree()
	{
		rootPass->Destroy();
	}

} // namespace CE::RPI
