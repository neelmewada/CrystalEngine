#include "CoreRPI.h"

namespace CE::RPI
{

    ParentPass::~ParentPass()
    {
		for (auto pass : passes)
		{
			pass->Destroy();
		}
		passes.Clear();
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

    Pass* ParentPass::GetPass(const Name& passName)
    {
		int index = passes.IndexOf([&](Pass* p) { return p->GetName() == passName; });
        if (index < 0)
            return nullptr;
        return passes[index];
    }

    void ParentPass::Clear()
    {
		passes.Clear();
    }

} // namespace CE::RPI
