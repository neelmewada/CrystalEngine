#pragma once

#define RPI_PASS(passName)\
	friend class Pass;\
	friend class PassTree;\
	friend class ParentPass;\
	friend class GpuPass;\
	friend class PassLibrary;\
	friend class PassSystem;\
	friend class RenderPipeline;

namespace CE::RPI
{
	class ParentPass;

	/// @brief The base Pass class. All passes should derive from this class.
	CLASS(Abstract)
	class CORERPI_API Pass : public Object
	{
		CE_CLASS(Pass, Object)
	public:
		friend class ParentPass;
		friend class PassTree;
		friend class PassRegistry;

		virtual ~Pass();

		virtual bool IsParentPass() const { return false; }

	protected:

		Pass();

		/// @brief Draw list tag this pass is associated to.
		DrawListTag drawListTag{};

		/// @brief The view tag associated with a pipeline view.
		/// The view that matches this tag will be queried by this pass.
		PipelineViewTag pipelineViewTag{};

		ParentPass* parentPass = nullptr;

		Array<PassAttachmentBinding> inputBindings{};
		Array<PassAttachmentBinding> inputOutputBindings{};
		Array<PassAttachmentBinding> outputBindings{};

		Array<Ptr<PassAttachment>> passAttachments{};

		friend class RenderPipeline;
	};

    
} // namespace CE::RPI

#include "Pass.rtti.h"
