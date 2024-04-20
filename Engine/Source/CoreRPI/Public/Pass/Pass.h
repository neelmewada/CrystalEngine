#pragma once

#if PAL_TRAIT_BUILD_TESTS
class RenderPipeline_DefaultPipelineTree_Test;
#endif

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

		virtual ~Pass();

		virtual bool IsParentPass() const { return false; }

		void AddAttachmentBinding(const PassAttachmentBinding& attachmentBinding);

		void AddSlot(const PassSlot& slot);

		PassAttachmentBinding* FindInputBinding(const Name& name);
		PassAttachmentBinding* FindInputOutputBinding(const Name& name);
		PassAttachmentBinding* FindOutputBinding(const Name& name);

		PassAttachmentBinding* FindBinding(const Name& name);

		const Name& GetPassName() const { return GetName(); }

	protected:

		Pass();

		/// @brief Draw list tag this pass is associated to.
		DrawListTag drawListTag{};

		/// @brief The view tag associated with a pipeline view.
		/// The view that matches this tag will be queried by this pass.
		PipelineViewTag pipelineViewTag{};

		FIELD()
		ParentPass* parentPass = nullptr;

		FIELD()
		Array<PassSlot> slots{};

		Array<PassAttachmentBinding> inputBindings{};
		Array<PassAttachmentBinding> inputOutputBindings{};
		Array<PassAttachmentBinding> outputBindings{};

		friend class RenderPipeline;

		RPI_PASS(Pass);

#if PAL_TRAIT_BUILD_TESTS
		friend class RenderPipeline_DefaultPipelineTree_Test;
#endif
	};

    
} // namespace CE::RPI

#include "Pass.rtti.h"
