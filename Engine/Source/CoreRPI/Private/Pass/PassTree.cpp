#include "CoreRPI.h"

namespace CE::RPI
{
	PassTree::PassTree()
	{

	}

	PassTree::~PassTree()
	{
		rootPass->Destroy();
	}

} // namespace CE::RPI
