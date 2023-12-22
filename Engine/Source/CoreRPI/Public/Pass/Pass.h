#pragma once

#define RPI_PASS(passName)\
	friend class Pass;\
	friend class PassFactory;\
	friend class PassLibrary;\
	friend class PassTemplate;\
	friend class PassSystem;\
	friend class RenderPipeline;

namespace CE::RPI
{
	constexpr u32 PassAttachmentBindingCountMax = 16;
	constexpr u32 PassInputBindingCountMax = PassAttachmentBindingCountMax;
	constexpr u32 PassOutputBindingCountMax = PassAttachmentBindingCountMax;
	constexpr u32 PassInputOutputBindingCountMax = PassAttachmentBindingCountMax;

	/// @brief The base Pass class. All passes should derive from this class.
	CLASS(Abstract)
	class CORERPI_API Pass : public Object
	{
		CE_CLASS(Pass, Object)
	public:
		friend class ParentPass;
		friend class PassFactory;
		friend class PassLibrary;

		virtual ~Pass();

	protected:

		Name name{};

		/// @brief The view tag associated with a pipeline view.
		/// The view that matches this tag will be queried by this pass.
		PipelineViewTag pipelineViewTag{};
		
	};

    
} // namespace CE::RPI

#include "Pass.rtti.h"
