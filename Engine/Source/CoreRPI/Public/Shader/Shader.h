#pragma once

namespace CE::RPI
{
	/// @brief RPI::Shader is effectively a collection of shader variants.
	class CORERPI_API Shader final
	{
	public:

		Shader();
		~Shader();

	private:

		Array<RPI::ShaderVariant*> variants{};

	};

} // namespace CE::RPI
