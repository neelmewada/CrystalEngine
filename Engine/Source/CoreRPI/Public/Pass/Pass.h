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

	/// @brief The base Pass class. All passes should derive from this class.
	class CORERPI_API Pass : public IntrusiveBase
	{
	public:

		virtual ~Pass();
		
		inline Name GetName() const { return name; }

	private:

		Name name{};
		
	};

	typedef Ptr<Pass> PassPtr;
    
} // namespace CE::RPI
