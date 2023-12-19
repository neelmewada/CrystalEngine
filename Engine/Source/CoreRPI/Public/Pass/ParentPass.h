#pragma once

namespace CE::RPI
{

	/// @brief The base Pass class. All passes should derive from this class.
	class CORERPI_API ParentPass : public Pass
	{
		friend class Pass;
		friend class PassFactory;
		friend class PassLibrary;
		friend class PassTemplate;
		friend class PassSystem;
	public:

		ParentPass();

		~ParentPass();

	private:

		Array<PassPtr> passes{};

	};
    
} // namespace CE::RPI
