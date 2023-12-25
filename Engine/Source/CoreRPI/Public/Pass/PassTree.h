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

	private:

		FIELD()
		ParentPass* rootPass = nullptr;

		friend class RenderPipeline;
	};

} // namespace CE::RPI

#include "PassTree.rtti.h"
