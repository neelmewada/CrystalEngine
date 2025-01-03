#include "CoreRPI.h"

namespace CE::RPI
{
	PassTree::PassTree()
	{
		rootPass = CreateDefaultSubobject<ParentPass>("RootPass");
	}

	PassTree::~PassTree()
	{
		
	}

    void PassTree::Clear()
    {
		for (Pass* pass : rootPass->passes)
		{
			pass->BeginDestroy();
		}
		
        rootPass->passes.Clear();
    }

    Pass* PassTree::FindPass(const Name& passName)
    {
        return FindPassInternal(passName, rootPass);
    }

	Pass* PassTree::GetPassAtPath(const Name& path, Pass* currentPassContext)
	{
		if (rootPass == nullptr)
			return nullptr;

		Array<String> split = path.GetString().RemoveWhitespaces().Split('.');
		if (split.IsEmpty())
			return nullptr;

		if (currentPassContext == nullptr)
			currentPassContext = rootPass;

		Pass* curSearchPass = nullptr;

		for (int i = 0; i < split.GetSize(); i++)
		{
			if (split[i].IsEmpty())
				continue;

			if (split[i][0] == '$') // Special name: $this, $parent, $root
			{
				if (split[i] == "$this")
				{
					curSearchPass = currentPassContext;
				}
				else if (split[i] == "$parent")
				{
					if (curSearchPass == nullptr)
						curSearchPass = currentPassContext;
					if (curSearchPass->parentPass == nullptr) // No parent exists, return null
						return nullptr;
					curSearchPass = curSearchPass->parentPass;
				}
				else if (split[i] == "$root")
				{
					curSearchPass = rootPass;
				}
			}
			else if (curSearchPass && curSearchPass->IsParentPass()) // Find the pass in current ParentPass
			{
                ParentPass* cast = (ParentPass*)curSearchPass;
				Pass* search = cast->GetPass(split[i]);
				if (search == nullptr) // Child pass not found, return null
					return nullptr;
				curSearchPass = search;
			}
			else if (curSearchPass == nullptr)
			{
				curSearchPass = FindPass(split[i]);
			}
		}

		return curSearchPass;
	}

	void PassTree::IterateRecursively(const Delegate<void(Pass*)>& functor)
	{
		if (rootPass == nullptr)
			return;

		IterateRecursively(rootPass, functor);
	}
	
	void PassTree::IterateRecursively(Pass* currentPass, const Delegate<void(Pass*)>& functor)
	{
		if (currentPass == nullptr)
			return;

		functor.InvokeIfValid(currentPass);

		if (currentPass->IsParentPass())
		{
			auto parentPass = (ParentPass*)currentPass;

			for (Pass* pass : parentPass->passes)
			{
				IterateRecursively(pass, functor);
			}
		}
	}

	Pass* PassTree::FindPassInternal(const Name& passName, ParentPass* parentPass)
	{
        if (parentPass->GetName() == passName)
            return parentPass;
        
		for (int i = 0; i < parentPass->passes.GetSize(); i++)
		{
			Pass* childPass = parentPass->passes[i];
			if (!childPass)
				continue;

			if (childPass->IsParentPass())
			{
				Pass* result = FindPassInternal(passName, (ParentPass*)childPass);
				if (result != nullptr)
					return result;
			}
			else if (childPass->GetName() == passName)
			{
				return childPass;
			}
		}
		return nullptr;
	}

} // namespace CE::RPI
