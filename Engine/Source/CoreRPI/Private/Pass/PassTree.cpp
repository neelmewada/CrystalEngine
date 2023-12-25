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

    void PassTree::Clear()
    {
        if (rootPass != nullptr)
        {
            rootPass->Destroy();
            rootPass = nullptr;
        }
    }

} // namespace CE::RPI
