#pragma once

namespace CE
{
	class Node;

	CLASS()
	class SYSTEM_API Scene : public Asset
	{
		CE_CLASS(Scene, Asset)
	public:

		Scene();
		virtual ~Scene();

		FORCE_INLINE Node* GetRoot() const { return root; }

	protected:

		FIELD(ReadOnly)
		Node* root = nullptr;
	};
    
} // namespace CE

#include "Scene.rtti.h"
