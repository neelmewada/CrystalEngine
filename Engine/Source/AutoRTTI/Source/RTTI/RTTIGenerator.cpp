
#include "RTTIGenerator.h"

namespace CE
{

	Array<IO::Path> RTTIGenerator::includeSearchPaths{};

	void RTTIGenerator::GenerateRTTI(String moduleName, IO::Path moduleHeaderRootPath, IO::Path outputPath, ModuleStamp& moduleStamp)
	{
		fs::path modulePath = moduleHeaderRootPath;

		ModuleAST moduleAST{ moduleName };

		std::stringstream implStream{};
		CE::Array<String> registrantList{};
		CE::Array<String> sourceHeaderPaths{};

		for (auto entry : fs::directory_iterator((fs::path)outputPath))
		{
			fs::remove(entry.path());
		}

		for (auto entry : fs::recursive_directory_iterator(modulePath))
		{
			if (entry.is_directory() || entry.path().extension() != ".h")
				continue;

			auto headerPath = entry.path();
			auto headerRelPath = fs::relative(headerPath, modulePath);
			fs::path headerGeneratedPath = (fs::path)outputPath / headerPath.filename().replace_extension(".rtti.h");

			std::ifstream inputHeaderFile{ entry.path(), std::ios_base::in };
			std::string inputHeaderFileContent((std::istreambuf_iterator<char>(inputHeaderFile)),
				(std::istreambuf_iterator<char>()));
			std::string searchString = std::string("#include \"") + headerPath.filename().replace_extension(".rtti.h").string() + "\"";

			if (inputHeaderFileContent.find(searchString) == std::string::npos)
			{
				continue;
			}

			auto crc = CE::CalculateCRC(inputHeaderFileContent.c_str(), inputHeaderFileContent.size());

			fs::path headerRelPathFinal{};
			int idx = 0;
			for (auto it = headerRelPath.begin(); it != headerRelPath.end(); ++it)
			{
				if (idx > 0 || (it->string() != "Public" && it->string() != "Private" && it->string() != "Sources"))
				{
					headerRelPathFinal /= *it;
				}
				idx++;
			}

			sourceHeaderPaths.Add(headerRelPathFinal.string());
            
            std::stringstream outStream{};

			moduleAST.ProcessHeader(headerPath, includeSearchPaths, outStream, implStream, registrantList);
			
			std::ofstream outFile{ headerGeneratedPath, std::ios_base::out };
			if (outFile.is_open())
			{
				outFile << outStream.str();
				outFile.close();
			}
		}

		std::ofstream moduleImplFile{ (fs::path)outputPath / (moduleName.ToStdString() + ".private.h"), std::ios_base::out };
		if (moduleImplFile.is_open())
		{
			moduleImplFile << "#pragma once\n\n";

			moduleImplFile << "#include \"CoreMinimal.h\"\n";

			for (auto headerPath : sourceHeaderPaths)
			{
				moduleImplFile << "#include \"" << headerPath.GetCString() <<"\"\n";
			}

			moduleImplFile << implStream.str() << "\n";

			moduleImplFile << "static void CERegisterModuleTypes()\n{\n";
			moduleImplFile << "\tCE_REGISTER_TYPES(\n";
			for (int i = 0; i < registrantList.GetSize(); i++)
			{
				moduleImplFile << "\t\t" << registrantList[i].GetCString();
				if (i < registrantList.GetSize() - 1)
					moduleImplFile << ",";
				moduleImplFile << "\n";
			}
			moduleImplFile << "\t);\n}\n";

			moduleImplFile.close();
		}
	}

} // namespace CE

CE_RTTI_STRUCT_IMPL(, CE, HeaderCRC)

CE_RTTI_STRUCT_IMPL(, CE, ModuleStamp)
