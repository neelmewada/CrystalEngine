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
			else if (curSearchPass && curSearchPass->IsParentPass())
			{
                ParentPass* cast = (ParentPass*)curSearchPass;
                
			}
		}

		return curSearchPass;
	}

	Pass* PassTree::FindPassInternal(const Name& passName, ParentPass* parentPass)
	{
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
