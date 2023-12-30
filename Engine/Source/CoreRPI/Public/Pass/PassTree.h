#pragma once

#if PAL_TRAIT_BUILD_TESTS
class RenderPipeline_DefaultPipelineTree_Test;
#endif

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

		inline ParentPass* GetRootPas() const
		{
			return rootPass;
		}

	private:

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
