
#include "EditorCore.h"

#include <locale>
#include <codecvt>
#include <string>

namespace CE::Editor
{
	static std::wstring ToWString(const std::string& stringToConvert)
	{
		std::wstring wideString = std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);
		return wideString;
	}

	static std::wstring ToWString(const String& string)
	{
		std::string stringToConvert = string.ToStdString();
		std::wstring wideString =
			std::wstring_convert<std::codecvt_utf8<wchar_t>>().from_bytes(stringToConvert);
		return wideString;
	}

	static std::wstring ToWString(const IO::Path& path)
	{
		return ToWString(path.GetString());
	}

	ShaderProcessor::ShaderProcessor()
	{

	}

	ShaderProcessor::~ShaderProcessor()
	{

	}

	void ShaderProcessor::AddIncludeSearchPath(const IO::Path& includePath)
	{
		includeSearchPaths.Add(includePath);
	}

	void ShaderProcessor::RemoveIncludeSearchPath(const IO::Path& includePath)
	{
		includeSearchPaths.Remove(includePath);
	}

	void ShaderProcessor::ClearIncludeSearchPaths()
	{
		includeSearchPaths.Clear();
	}

	ShaderProcessor::ErrorCode ShaderProcessor::ProcessHLSL(const IO::Path& filePath, IO::Path outPath, const ShaderBuildConfig& buildConfig)
	{
		ShaderCompiler compiler{};
		ShaderReflector reflector{};

		Array<std::wstring> baseArgs{
			L"-spirv"
		};

		for (auto& includePath : includeSearchPaths)
		{
			baseArgs.Add(L"-I");
			baseArgs.Add(ToWString(includePath));
		}

		if (outPath.GetExtension().IsEmpty()) // outPath has no extension, add .csbin to end
		{
			outPath = outPath.GetString() + ".csbin";
		}

		if (!outPath.GetParentPath().Exists())
		{
			IO::Path::CreateDirectories(outPath.GetParentPath());
		}
        
        Archive shaderArchive{};
        shaderArchive.Open(outPath, ArchiveMode::Write);
		
		Array<std::wstring> vertArgs{};
		vertArgs.AddRange(baseArgs);
		vertArgs.AddRange({
			L"-E", ToWString(buildConfig.vertEntry),
			L"-T", L"vs_6_0",
			L"-D", L"VERTEX=1"
		});

		void* byteCode = nullptr;
		u32 byteSize = 0;
        
        auto result = compiler.Compile(filePath, vertArgs, &byteCode, &byteSize);
        
        if (result != ShaderCompiler::ERR_Success)
        {
            errorMessage = compiler.GetErrorMessage();
            shaderArchive.Close();
            IO::Path::Remove(outPath);
            return ERR_VertexFail;
        }
        
        // Write Vertex binary
        shaderArchive.OpenEntry("bytecode0_vert.spv");
        {
            shaderArchive.EntryWrite(byteCode, byteSize);
        }
        shaderArchive.CloseEntry();

		// Vertex Reflection

        
        Memory::Free(byteCode);
        byteCode = nullptr;
        byteSize = 0;

		Array<std::wstring> fragArgs{};
		fragArgs.AddRange(baseArgs);
		fragArgs.AddRange({
			L"-E", ToWString(buildConfig.fragEntry),
			L"-T", L"ps_6_0",
			L"-D", L"FRAGMENT=1"
		});
        
        result = compiler.Compile(filePath, fragArgs, &byteCode, &byteSize);
        
        if (result != ShaderCompiler::ERR_Success)
        {
            errorMessage = compiler.GetErrorMessage();
            shaderArchive.Close();
            IO::Path::Remove(outPath);
            return ERR_FragmentFail;
        }
        
		// Write Fragment binary
        shaderArchive.OpenEntry("bytecode0_frag.spv");
        {
            shaderArchive.EntryWrite(byteCode, byteSize);
        }
        shaderArchive.CloseEntry();

		// Fragment Reflection
        
        Memory::Free(byteCode);
        byteCode = nullptr;
        byteSize = 0;
        
        shaderArchive.Close();

		return ERR_Success;
	}

} // namespace CE::Editor

