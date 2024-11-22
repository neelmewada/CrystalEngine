#include "CoreRPI.h"

namespace CE::RPI
{

    ParentPass::~ParentPass()
    {

    }

	void ParentPass::AddChild(Pass* childPass)
	{
		if (!passes.Exists(childPass))
		{
			passes.Add(childPass);
			childPass->renderPipeline = renderPipeline;
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
		for (Pass* pass : passes)
		{
			pass->BeginDestroy();
		}
		passes.Clear();
    }

} // namespace CE::RPI
