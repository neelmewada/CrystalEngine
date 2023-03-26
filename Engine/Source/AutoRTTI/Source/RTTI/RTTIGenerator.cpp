
#include "RTTIGenerator.h"

namespace CE
{

	Array<IO::Path> RTTIGenerator::includeSearchPaths{};

	void RTTIGenerator::GenerateRTTI(String moduleName, IO::Path moduleHeaderRootPath, IO::Path outputPath)
	{
		fs::path modulePath = moduleHeaderRootPath;

		ModuleAST moduleAST{ moduleName };

		for (auto entry : fs::recursive_directory_iterator(modulePath))
		{
			if (entry.is_directory() || entry.path().extension() != ".h")
				continue;

			if (entry.path().filename() != "TextureAsset.h")
			{
				continue; // TODO: Only for testing
			}
            
            std::stringstream outStream{};

			moduleAST.ProcessHeader(entry.path(), includeSearchPaths, outStream);
		}
	}

} // namespace CE

