#pragma once

namespace CE::RPI
{
    
	class CORERPI_API PassRegistry final
	{
		CE_NO_COPY(PassRegistry)
	public:

		static PassRegistry& Get();

		SubClass<Pass> GetPassClass(const Name& shortName);

		void OnClassRegistered(ClassType* classType);
		void OnClassDeregistered(ClassType* classType);

	private:

		struct Entry
		{
			Name shortName{};
			SubClass<Pass> passClass = nullptr;
		};

		using RegistryMap = HashMap<Name, Entry>;

		PassRegistry() = default;

		RegistryMap registry{};

	};

} // namespace CE::RPI
