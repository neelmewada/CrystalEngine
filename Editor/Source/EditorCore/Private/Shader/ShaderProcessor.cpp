
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

	ShaderProcessor::ErrorCode ShaderProcessor::ProcessHLSL(const IO::Path& filePath, const IO::Path& outPath, const ShaderBuildConfig& buildConfig)
	{
		ShaderCompiler compiler{};

		Array<std::wstring> baseArgs{
			L"-spirv"
		};

		for (auto& includePath : includeSearchPaths)
		{
			baseArgs.Add(L"-I");
			baseArgs.Add(ToWString(includePath));
		}
		
		Array<std::wstring> vertArgs{};
		vertArgs.AddRange(baseArgs);
		vertArgs.AddRange({
			L"-E", ToWString(buildConfig.vertEntry),
			L"-T", L"vs_6_0",
			L"-D", L"VERTEX=1"
		});

		void* byteCode = nullptr;
		u32 byteSize = 0;
		// TODO: temporary code!
		if (compiler.Compile(filePath, vertArgs, &byteCode, &byteSize) == ShaderCompiler::ERR_Success)
		{
			std::fstream fileStream{ outPath, std::ios::out | std::ios::binary };
			fileStream.write((char*)byteCode, byteSize);
			fileStream.close();
			Memory::Free(byteCode);
		}
		else
		{
			errorMessage = compiler.GetErrorMessage();
			return ERR_VertexFail;
		}
		return ERR_Success;

		Array<std::wstring> fragArgs{};
		fragArgs.AddRange(baseArgs);
		fragArgs.AddRange({
			L"-E", ToWString(buildConfig.fragEntry),
			L"-T", L"ps_6_0",
			L"-D", L"FRAGMENT=1"
		});

		return ERR_Success;
	}

} // namespace CE::Editor

