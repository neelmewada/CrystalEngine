#include "CoreRPI.h"

namespace CE::RPI
{
    ParentPass::ParentPass()
    {

    }

    ParentPass::~ParentPass()
    {

    }

	void ParentPass::AddChild(Pass* childPass)
	{
		if (!passes.Exists(childPass))
		{
			passes.Add(childPass);
		}
	}

	void ParentPass::RemoveChild(Pass* childPass)
	{
		passes.Remove(childPass);
	}

} // namespace CE::RPI
