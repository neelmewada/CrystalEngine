#pragma once

#if PAL_TRAIT_BUILD_TESTS
class RenderPipeline_DefaultPipelineTree_Test;
#endif

namespace CE::RPI
{
	class RenderPipeline;
	
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

		inline ParentPass* GetRootPass() const
		{
			return rootPass;
		}

    	void IterateRecursively(const Delegate<void(Pass*)>& functor);

	private:

    	void IterateRecursively(Pass* currentPass, const Delegate<void(Pass*)>& functor);

		Pass* FindPassInternal(const Name& passName, ParentPass* parentPass);

		FIELD()
		ParentPass* rootPass = nullptr;

		friend class RenderPipeline;

#if PAL_TRAIT_BUILD_TESTS
		friend class RenderPipeline_DefaultPipelineTree_Test;
#endif
	};

} // namespace CE::RPI

#include "PassTree.rtti.h"
