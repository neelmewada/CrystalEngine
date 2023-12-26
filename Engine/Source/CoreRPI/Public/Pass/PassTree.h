#pragma once

namespace CE::RPI
{
    CLASS()
	class CORERPI_API PassTree final : public Object
	{
		CE_CLASS(PassTree, Object)
	public:

		PassTree();

		virtual ~PassTree();
        
        void Clear();

		Pass* FindPass(const Name& passName);

		/// @brief Find and return the pass by the path.
		/// @param path to the pass to return. Ex: $root.DepthPrePassParent.DepthPrePass
		/// @param currentPassContext denotes the pass defined by `$this` key.
		Pass* GetPassAtPath(const Name& path, Pass* currentPassContext = nullptr);

	private:

		Pass* FindPassInternal(const Name& passName, ParentPass* parentPass);

		FIELD()
		ParentPass* rootPass = nullptr;

		friend class RenderPipeline;
	};

} // namespace CE::RPI

#include "PassTree.rtti.h"
