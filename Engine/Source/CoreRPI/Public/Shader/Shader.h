#pragma once

namespace CE::RPI
{
	/// @brief Shader is effectively an 'uber shader', a collection of shader variants.
	class CORERPI_API Shader final
	{
	public:

	private:

		Array<RPI::ShaderVariant*> variants{};

	};

} // namespace CE::RPI
