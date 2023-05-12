
#include "CoreMinimal.h"
#include "EditorCore.h"

#include "cxxopts.hpp"

#define LOG(x) std::cout << x << std::endl;
#define LOG_ERROR(x) std::cerr << "FATAL: " << x << std::endl;

int main(int argc, const char** argv)
{
	using namespace CE;
	Logger::Initialize();

	cxxopts::Options options("CrystalShaderTool", "Crystal Engine's shader compilation tool. Compiles HLSL code into a Crystal Shader unit with reflection");
	
	options.add_options()
		("h,help", "Show this help text")
		("i,input", "List of input HLSL files to compile", cxxopts::value<std::vector<std::string>>())
		("o,output", "Output binary shader package", cxxopts::value<std::string>());
	;

	options.allow_unrecognised_options();
	
	try
	{
		auto result = options.parse(argc, argv);

		if (result["h"].as<bool>())
		{
			LOG(options.help());
			return 0;
		}

		if (result["input"].count() == 0)
		{
			LOG_ERROR("No input HLSL files passed");
			return 0;
		}
		
		using namespace Editor;

		ShaderCompiler compiler{ };
		
	}
	catch (std::exception exc)
	{
		LOG_ERROR("Failed to parse arguments");
		LOG(options.help());
		return -1;
	}

	Logger::Shutdown();
	return 0;
}
