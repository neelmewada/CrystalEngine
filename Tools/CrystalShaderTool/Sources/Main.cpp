
#include "CoreMinimal.h"
#include "EditorCore.h"

#include "cxxopts.hpp"

#define LOG(x) std::cout << x << std::endl;
#define LOG_ERROR(x) std::cerr << "FATAL: " << x << std::endl;

int main(int argc, const char** argv)
{
	using namespace CE;
	Logger::Initialize();

	cxxopts::Options options("CrystalShaderTool", "Crystal Engine's shader compilation tool. Compiles HLSL code into a Crystal Shader binary package with reflection");
	
	options.add_options()
		("h,help", "Show this help text")
		("i,input", "List of input HLSL files to compile", cxxopts::value<std::string>())
		("o,output", "Output binary shader package", cxxopts::value<std::string>())
		("spirv", "Build spirv binary", cxxopts::value<bool>()->default_value("true"))
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
			return 1;
		}

		IO::Path inputFile = result["input"].as<std::string>();
		if (!inputFile.Exists())
		{
			LOG_ERROR("Input file does not exist")
			return 2;
		}

		IO::Path outputFile = result["output"].as<std::string>();
		if (outputFile.Exists())
		{
			IO::Path::Remove(outputFile);
		}
		
		using namespace Editor;

		ShaderProcessor processor{};

		ShaderBuildConfig config{};
		config.debugName = "TestShader";
		config.vertEntry = "VertMain";
		config.fragEntry = "FragMain";

		auto code = processor.ProcessHLSL(inputFile, outputFile, config);

		if (code != ShaderProcessor::ERR_Success)
		{
			LOG_ERROR("Compilation failed. Error:\n" << processor.GetErrorMessage().ToStdString());
			return 3;
		}
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
