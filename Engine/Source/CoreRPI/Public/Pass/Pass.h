#pragma once

#define RPI_PASS(passName)\
	friend class Pass;\
	friend class PassFactory;\
	friend class PassLibrary;\
	friend class PassTemplate;\
	friend class PassSystem;\
	friend class ParentPass;\
	friend class RenderPipeline;

namespace CE::RPI
{
	constexpr u32 PassAttachmentBindingCountMax = 16;
	constexpr u32 PassInputBindingCountMax = PassAttachmentBindingCountMax;
	constexpr u32 PassOutputBindingCountMax = PassAttachmentBindingCountMax;
	constexpr u32 PassInputOutputBindingCountMax = PassAttachmentBindingCountMax;

	/// @brief The base Pass class. All passes should derive from this class.
	class CORERPI_API Pass : public IntrusiveBase
	{
	public:

		virtual ~Pass();
		
		inline Name GetName() const { return name; }

		virtual Name GetClassName() const { return "Pass"; }

	private:

		Name name{};

		/// @brief The view tag associated with a pipeline view.
		/// The view that matches this tag will queried by this pass.
		PipelineViewTag pipelineViewTag{};
		
	};

	typedef Ptr<Pass> PassPtr;

	using PassCreatorFunc = Delegate<PassPtr(const PassDesc&)>;
    
} // namespace CE::RPI
