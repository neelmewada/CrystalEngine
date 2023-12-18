#pragma once

namespace CE::RPI
{
	/// @brief The base Pass class. All passes should derive from this class.
	class CORERPI_API Pass : public IntrusiveBase
	{
	public:

		virtual ~Pass();

	private:

	};
    
} // namespace CE::RPI
