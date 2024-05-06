#pragma once

namespace CE::RPI
{

	class CORERPI_API MaterialSystem final
	{
		CE_NO_COPY(MaterialSystem)
	public:

		static MaterialSystem& Get();

		void Update(u32 imageIndex);

	private:

		MaterialSystem() = default;

		~MaterialSystem() = default;

		List<RPI::Material*> materials{};

		friend class Material;
	};

} // namespace CE::RPI
