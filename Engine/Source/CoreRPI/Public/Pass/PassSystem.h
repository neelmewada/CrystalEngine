#pragma once

namespace CE::RPI
{
	class RasterPass;

	class CORERPI_API PassSystem
	{
	public:

		void Initialize();

		void Shutdown();

		static PassSystem& Get();

		void RegisterTemplate(Name templateName, Pass* pass);

		void RegisterTemplate(Pass* pass);

		Pass* CreatePass(Name templateName, Name newPassName = "");

	private:

		PassSystem() = default;

		HashMap<Name, Pass*> passTemplates{};
	};
    
} // namespace CE::RPI
