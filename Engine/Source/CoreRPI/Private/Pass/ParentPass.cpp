#include "CoreRPI.h"

namespace CE::RPI
{
    ParentPass::ParentPass(const PassDesc& desc) : desc(desc)
    {
		name = desc.passName;
		
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

    void ParentPass::Clear()
    {
		passes.Clear();
    }

} // namespace CE::RPI
