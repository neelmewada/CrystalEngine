#pragma once

namespace CE
{

	class CORE_API EngineDirectories
	{
		CE_STATIC_CLASS(EngineDirectories)
	public:

		static IO::Path GetEngineInstallDirectory();

		static IO::Path GetGameInstallDirectory();

		//! @brief Returns the path to ./Game directory in the project or installed game. 
		static IO::Path GetGameDirectory();

	};

} // namespace CE
