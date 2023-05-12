
#include "CoreMinimal.h"
#include "EditorCore.h"

#include "cxxopts.hpp"

#define LOG(x) std::cout << x << std::endl;
#define LOG_ERROR(x) std::cerr << x << std::endl;

int main(int argc, const char** argv)
{
	using namespace CE;
	Logger::Initialize();

	cxxopts::Options options("CrystalShaderTool", "Crystal Engine's shader compilation tool. Compiles HLSL code into a Crystal Shader unit with reflection");
	
	options.add_options()
		("h,help", "Show this help text")
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
		
		
	}
	catch (std::exception exc)
	{
		LOG_ERROR("ERROR: Failed to parse arguments");
		LOG(options.help());
		return -1;
	}

	Logger::Shutdown();
	return 0;
}
